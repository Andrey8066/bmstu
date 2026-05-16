#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include "SharedLog.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, SharedLog &log, boost::asio::thread_pool &workers)
      : socket_(std::move(socket)), log_(log), workers_(workers),
        timer_(socket_.get_executor()) {}

  void start() {
    start_timeout();
    read();
  }

private:
  unsigned long long factorial(unsigned int n) {
    std::this_thread::sleep_for(std::chrono::seconds(2));

    unsigned long long result = 1;

    for (unsigned int i = 1; i <= n; ++i)
      result *= i;

    return result;
  }

  void start_timeout() {
    timer_.expires_after(std::chrono::seconds(30));

    auto self = shared_from_this();

    timer_.async_wait([this, self](const boost::system::error_code &ec) {
      if (!ec) {
        std::cout << "Client timeout\n";

        boost::system::error_code ignored;
        socket_.close(ignored);
      }
    });
  }

  void read() {
    auto self = shared_from_this();

    boost::asio::async_read_until(
        socket_, boost::asio::dynamic_buffer(buffer_), '\n',
        [this, self](boost::system::error_code ec, std::size_t) {
          if (ec) {
            if (ec != boost::asio::error::operation_aborted)
              std::cerr << "Read error: " << ec.message() << std::endl;

            return;
          }

          timer_.cancel();

          std::istringstream iss(buffer_);
          unsigned int n;
          iss >> n;

          buffer_.clear();

          process(n);
        });
  }

  void process(unsigned int n) {
    auto self = shared_from_this();

    boost::asio::post(workers_, [this, self, n]() {
      try {
        auto result = factorial(n);

        std::ostringstream log_msg;
        log_msg << "factorial(" << n << ") = " << result;

        log_.add(log_msg.str());

        std::ostringstream response;
        response << result << "\n";

        boost::asio::post(
            socket_.get_executor(),
            [this, self, response = response.str()]() { write(response); });
      } catch (const std::exception &ex) {
        std::cerr << "Processing error: " << ex.what() << std::endl;
      }
    });
  }

  void write(const std::string &message) {
    auto self = shared_from_this();

    boost::asio::async_write(
        socket_, boost::asio::buffer(message),
        [this, self](boost::system::error_code ec, std::size_t) {
          if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
          } else {
            start_timeout();
            read();
          }
        });
  }

private:
  tcp::socket socket_;

  SharedLog &log_;

  boost::asio::thread_pool &workers_;

  boost::asio::steady_timer timer_;

  std::string buffer_;
};
