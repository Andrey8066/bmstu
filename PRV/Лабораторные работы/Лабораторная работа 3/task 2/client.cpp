#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io;

    tcp::socket socket(io);
    tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345);

    socket.connect(endpoint);

    std::cout << "Подключено к серверу.\n";

    std::cout << "Введите строку: ";
    std::string message;
    std::getline(std::cin, message);

    message += "\n"; // обязательно!

    boost::asio::write(socket, boost::asio::buffer(message));

    boost::asio::streambuf buffer;
    boost::asio::read_until(socket, buffer, '\n');

    std::istream input(&buffer);
    std::string response;
    std::getline(input, response);

    std::cout << "Ответ сервера: " << response << std::endl;

  } catch (std::exception &e) {
    std::cerr << "Ошибка клиента: " << e.what() << std::endl;
  }
}