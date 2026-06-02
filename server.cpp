#include "./include/Server.hpp"
#include <iostream>

const short PORT = 8080;

int main() {
  try {
    Server chatServer(PORT);
    chatServer.start();

    std::cout << "Server running. Press Enter to stop.\n";
    std::cin.ignore();

    chatServer.stop();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
  }
  return 0;
}
