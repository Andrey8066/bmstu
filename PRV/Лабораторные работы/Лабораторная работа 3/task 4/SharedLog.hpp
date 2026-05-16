#include <boost/asio.hpp>
#include <iostream>
#include <vector>

using boost::asio::ip::tcp;

class SharedLog {
public:
  explicit SharedLog(boost::asio::io_context &io)
      : strand_(boost::asio::make_strand(io)) {}

  void add(std::string message) {
    boost::asio::dispatch(strand_, [this, message = std::move(message)]() {
      log_.push_back(message);

      std::cout << "[LOG] " << log_.back() << std::endl;
    });
  }

private:
  std::vector<std::string> log_;

  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
};
