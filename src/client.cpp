#include "../include/Client.hpp"
#include <chrono>
#include <iostream>
#include <thread>

Client::Client(const std::string &serverIp, short port)
    : connectionHandler(std::make_unique<ConnectionHandler>(serverIp, port)) {}

void Client::start() {
  std::cout << "Client starting...\n";
  connectionHandler->connect();
  // In a real application, UI would be initialized here
}

void Client::stop() {
  std::cout << "Client stopping...\n";
  connectionHandler->disconnect();
}

void Client::login(const std::string &user) {
  username = user;
  connectionHandler->send(MessageType::LOGIN, username);
  auto [type, payload] = connectionHandler->receive();
  std::cout << "Server: " << payload << "\n";
}

void Client::sendMessage(const std::string &message) {
  connectionHandler->send(MessageType::MESSAGE, message);
  auto [type, payload] = connectionHandler->receive();
  std::cout << "Server: " << payload << "\n";
}
