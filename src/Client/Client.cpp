#include "Client/Client.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

Client::Client(const std::string &serverIp, short port)
    : connectionHandler(std::make_unique<ConnectionHandler>(serverIp, port)) {}

void Client::start() {
  std::cout << "\033[34mClient starting...\033[0m" << '\n';
  connectionHandler->connect();
  receiving = true;
  receiveThread = std::thread(&Client::receiveLoop, this);
}

void Client::stop() {
  std::cout << "\033[34mClient stopping...\033[0m" << '\n';
  connectionHandler->send(MessageType::LOGOUT, username);
  receiving = false;
  if (receiveThread.joinable()) {
    receiveThread.join();
  }
  connectionHandler->disconnect();
}

void Client::login(const std::string &user) {
  username = user;
  connectionHandler->send(MessageType::LOGIN, username);
}

void Client::sendMessage(const std::string &message) {
  connectionHandler->send(MessageType::MESSAGE, message);
}

void Client::receiveLoop() {
  while (receiving) {
    try {
      auto [type, payload] = connectionHandler->receive();
      if (type == MessageType::MESSAGE) {
        std::cout << "\n" << payload << "\n";
        std::cout << "Enter message: " << std::flush;
      }
    } catch (const std::exception &e) {
      if (receiving) {
        std::cerr << "Error receiving message: " << e.what() << '\n';
      }
      break;
    }
  }
}
