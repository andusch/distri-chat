#include "../include/ClientConnection.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

ClientConnection::ClientConnection(int sock) : clientSocket(sock) {}

ClientConnection::~ClientConnection() {
  if (clientSocket != -1) {
    close(clientSocket);
    std::cout << "Client socket closed for user: " << connectedUsername << '\n';
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
    std::cerr << "Error: " << e.what() << '\n';
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
    std::cout << "User logged in: " << connectedUsername << "\n";
    send(MessageType::MESSAGE, "Welcome, " + connectedUsername + "!");
    break;
  case MessageType::MESSAGE:
    std::cout << "Message from " << connectedUsername << ": " << payload
              << "\n";
    send(MessageType::MESSAGE, payload); // Echo back for now
    break;
  case MessageType::LOGOUT:
    std::cout << "User logged out: " << payload << "\n";
    // Perform cleanup if necessary
    break;
  default:
    std::cerr << "Unknown message type received.\n";
    break;
  }
}
