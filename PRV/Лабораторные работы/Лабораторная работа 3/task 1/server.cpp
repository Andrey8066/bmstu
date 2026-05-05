#include <algorithm>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

// Функция перевода строки в верхний регистр
std::string to_upper(const std::string &input) {
  std::string result = input;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

int main() {
  try {
    boost::asio::io_context io;

    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));
    std::cout << "Сервер запущен на порту 12345...\n";

    while (true) {
      tcp::socket socket(io);
      acceptor.accept(socket);

      std::cout << "Клиент подключен: " << socket.remote_endpoint()
                << std::endl;

      boost::asio::streambuf buffer;

      // читаем строку до \n
      boost::asio::read_until(socket, buffer, '\n');

      std::istream input(&buffer);
      std::string message;
      std::getline(input, message);

      std::cout << "Получено: " << message << std::endl;

      // обработка
      std::string upper = to_upper(message);
      std::string response =
          std::to_string(message.size()) + ": " + upper + "\n";

      // отправка
      boost::asio::write(socket, boost::asio::buffer(response));

      std::cout << "Отправлено: " << response << std::endl;
    }

  } catch (std::exception &e) {
    std::cerr << "Ошибка сервера: " << e.what() << std::endl;
  }
}