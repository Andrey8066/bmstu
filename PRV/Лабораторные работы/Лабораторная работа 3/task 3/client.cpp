#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <memory>
#include <thread>

using boost::asio::ip::tcp;

class Client : public std::enable_shared_from_this<Client> {
public:
  Client(boost::asio::io_context &io) : resolver_(io), socket_(io), io_(io) {}

  void start(const std::string &host, const std::string &port) {
    do_resolve(host, port);
  }

  void write(const std::string &msg) {
    auto self = shared_from_this();

    boost::asio::post(io_, [this, self, msg]() {
      bool writing = !write_msgs_.empty();
      write_msgs_.push_back(msg + "\n");

      if (!writing) {
        do_write();
      }
    });
  }

private:
  void do_resolve(const std::string &host, const std::string &port) {
    auto self = shared_from_this();

    resolver_.async_resolve(
        host, port,
        [this, self](boost::system::error_code ec,
                     tcp::resolver::results_type endpoints) {
          if (!ec) {
            do_connect(endpoints);
          } else {
            std::cerr << "Resolve error: " << ec.message() << std::endl;
          }
        });
  }

  void do_connect(const tcp::resolver::results_type &endpoints) {
    auto self = shared_from_this();

    boost::asio::async_connect(
        socket_, endpoints,
        [this, self](boost::system::error_code ec, tcp::endpoint) {
          if (!ec) {
            std::cout << "Подключено к серверу\n";
            do_read();
          } else {
            std::cerr << "Connect error: " << ec.message() << std::endl;
          }
        });
  }

  void do_read() {
    auto self = shared_from_this();

    boost::asio::async_read_until(
        socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);

            std::cout << "Ответ: " << line << std::endl;

            do_read(); // продолжаем слушать
          } else {
            if (ec != boost::asio::error::eof) {
              std::cerr << "Read error: " << ec.message() << std::endl;
            }
          }
        });
  }

  void do_write() {
    auto self = shared_from_this();

    boost::asio::async_write(
        socket_, boost::asio::buffer(write_msgs_.front()),
        [this, self](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
              do_write();
            }
          } else {
            std::cerr << "Write error: " << ec.message() << std::endl;
          }
        });
  }

private:
  tcp::resolver resolver_;
  tcp::socket socket_;
  boost::asio::streambuf buffer_;
  std::deque<std::string> write_msgs_;
  boost::asio::io_context &io_;
};

int main() {
  try {
    boost::asio::io_context io;

    auto client = std::make_shared<Client>(io);
    client->start("127.0.0.1", "12345"); // 🔥 запускаем правильно

    // поток для ввода
    std::thread input_thread([client]() {
      std::string line;
      while (std::getline(std::cin, line)) {
        client->write(line);
      }
    });

    io.run();
    input_thread.join();

  } catch (std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
  }
}