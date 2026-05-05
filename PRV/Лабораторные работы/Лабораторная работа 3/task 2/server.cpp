#include <algorithm>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <vector>

using boost::asio::ip::tcp;

// функция поиска максимума
int find_max(const std::string &input) {
  std::istringstream iss(input);
  std::vector<int> nums;
  int x;

  while (iss >> x) {
    nums.push_back(x);
  }

  if (nums.empty())
    return 0;

  return *std::max_element(nums.begin(), nums.end());
}

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

      // ВАЖНО: переносим socket
      boost::asio::post(io, [sock = std::move(socket), message]() mutable {
        int max_val = find_max(message);

        std::string response = "Максимум: " + std::to_string(max_val) + "\n";

        std::cout << response << std::endl;

        boost::asio::write(sock, boost::asio::buffer(response));
      });

      io.run();
      io.restart(); // чтобы можно было снова run()
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}