#include "Server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char *argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: server <threads>\n";
      return 1;
    }

    const int thread_count = std::stoi(argv[1]);

    boost::asio::io_context io;

    boost::asio::thread_pool workers(thread_count);

    Server server(io, 12345, workers);

    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i) {
      threads.emplace_back([&io]() { io.run(); });
    }

    for (auto &t : threads)
      t.join();

    workers.join();
  } catch (const std::exception &ex) {
    std::cerr << "Fatal error: " << ex.what() << std::endl;
  }

  return 0;
}