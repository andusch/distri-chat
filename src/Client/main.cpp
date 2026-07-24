#include "Client/Client.hpp"
#include <algorithm>
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

    while (true) {

      std::string message;
      std::cout << "Enter message: " << std::flush;
      std::getline(std::cin, message);

      std::string aux = message;
      std::transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
      // std::cout << "Lowercase message is: " << aux << '\n';
      if ((aux.compare(std::string("exit")) == 0) ||
          (aux.compare(std::string("stop")) == 0) ||
          (aux.compare(std::string("disconnect")) == 0)) {
        break;
      }
      chatClient.sendMessage(message);
    }

    chatClient.stop();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
  }
}
