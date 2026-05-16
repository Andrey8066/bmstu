#include "Session.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
using boost::asio::ip::tcp;

class Server {
public:
  Server(boost::asio::io_context &io, unsigned short port,
         boost::asio::thread_pool &workers)
      : acceptor_(io, tcp::endpoint(tcp::v4(), port)), log_(io),
        workers_(workers) {
    accept();
  }

private:
  void accept() {
    acceptor_.async_accept([this](boost::system::error_code ec,
                                  tcp::socket socket) {
      if (!ec) {
        std::cout << "Client connected\n";

        std::make_shared<Session>(std::move(socket), log_, workers_)->start();
      } else {
        std::cerr << "Accept error: " << ec.message() << std::endl;
      }

      accept();
    });
  }

private:
  tcp::acceptor acceptor_;

  SharedLog log_;

  boost::asio::thread_pool &workers_;
};