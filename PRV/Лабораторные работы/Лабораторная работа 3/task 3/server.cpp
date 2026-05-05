#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io;

    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));
    std::cout << "Сервер запущен...\n";

    while (true) {
      tcp::socket socket(io);
      acceptor.accept(socket);

      boost::asio::streambuf buffer;
      boost::asio::read_until(socket, buffer, '\n');

      std::istream input(&buffer);
      std::string message;
      std::getline(input, message);

      std::cout << "Получено: " << message << std::endl;

      if (message.rfind("timer", 0) == 0) {
        std::istringstream iss(message);
        std::string cmd;
        int seconds;
        iss >> cmd >> seconds;

        // отправляем сразу
        std::string response = "Ready in " + std::to_string(seconds) + " sec\n";

        boost::asio::write(socket, boost::asio::buffer(response));

        std::cout << "Отправлено: " << response;

        // ВАЖНО: переносим socket в shared_ptr
        auto sock = std::make_shared<tcp::socket>(std::move(socket));

        // ВАЖНО: таймер тоже в shared_ptr
        auto timer = std::make_shared<boost::asio::steady_timer>(
            io, boost::asio::chrono::seconds(seconds));

        timer->async_wait([sock, timer](const boost::system::error_code &ec) {
          if (!ec) {
            std::string msg = "Done!\n";
            boost::asio::write(*sock, boost::asio::buffer(msg));

            std::cout << "Отправлено: Done!\n";
          }
        });

        // запускаем обработку async
        io.run();
        io.restart();
      }
    }

  } catch (std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << std::endl;
  }
}