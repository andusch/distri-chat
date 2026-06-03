#include "../include/ClientConnection.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

ClientConnection::ClientConnection(int sock) : clientSocket(sock) {}

ClientConnection::~ClientConnection() {
  if (clientSocket != -1) {
    close(clientSocket);
    std::cout << "\033[34mClient socket closed for user: \033[32m"
              << connectedUsername << "\033[0m" << '\n';
  }
}

void ClientConnection::handleClient() {

  try {
    while (true) {
      auto [type, payload] = receive();
      processMessage(type, payload);
      if (type == MessageType::LOGOUT) {
        break;
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "\033[31mError: " << e.what() << "\033[0m" << '\n';
  }
}

void ClientConnection::send(MessageType type, const std::string &payload) {
  auto buffer = serialize_message(type, payload);
  ::send(clientSocket, buffer.data(), buffer.size(), 0);
}

std::pair<MessageType, std::string> ClientConnection::receive() {
  return receive_message(clientSocket);
}

void ClientConnection::processMessage(MessageType type,
                                      const std::string &payload) {
  switch (type) {
  case MessageType::LOGIN:
    connectedUsername = payload;
    std::cout << "User logged in: \033[32m" << connectedUsername << "\033[0m"
              << '\n';
    send(MessageType::MESSAGE, "Welcome, " + connectedUsername + "!");
    break;
  case MessageType::MESSAGE:
    std::cout << "Message from \033[32m" << connectedUsername
              << "\033[0m: " << payload << "\033[0m" << '\n';
    send(MessageType::MESSAGE, payload); // Echo back for now
    break;
  case MessageType::LOGOUT:
    std::cout << "User logged out: \033[32m" << payload << "\033[0m" << '\n';
    // Perform cleanup if necessary
    break;
  default:
    std::cerr << "\033[31mUnknown message type received.\033[0m" << '\n';
    break;
  }
}
