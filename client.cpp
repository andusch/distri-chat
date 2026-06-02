#include "./include/Client.hpp"
#include <cstring>
#include <iostream>

const short PORT = 8080;
const char *SERVER_IP = "127.0.0.1"; // loop-back address

int main() {
  try {

    Client chatClient(SERVER_IP, PORT);
    chatClient.start();

    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    chatClient.login(username);

    std::string message;
    std::cout << "Enter message: ";
    std::getline(std::cin, message);
    chatClient.sendMessage(message);

    chatClient.stop();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
  }
}
