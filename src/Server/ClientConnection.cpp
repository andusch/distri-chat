#include "Server/ClientConnection.hpp"
#include "Server/Server.hpp"
#include <cstring>
#include <iostream>
#include <mutex>
#include <unistd.h>

ClientConnection::ClientConnection(int sock, Server &s)
    : clientSocket(sock), server(s), connected(true) {}

ClientConnection::~ClientConnection() {
  if (clientSocket != -1) {
    close(clientSocket);
    std::cout << "\033[34mClient socket closed for user: \033[32m"
              << connectedUsername << "\033[0m" << '\n';
    std::lock_guard<std::mutex> lock(server.clientsMutex);
    server.activeClients.erase(connectedUsername);
  }
}

void ClientConnection::disconnect() {
  if (clientSocket != -1 && connected) {
    close(clientSocket);
    clientSocket = -1;
    connected = false;

    if (!connectedUsername.empty()) {
      std::lock_guard<std::mutex> lock(server.clientsMutex);
      server.activeClients.erase(connectedUsername);

      std::cout << "Client socket closed for user: " << connectedUsername
                << '\n';
      server.broadcastMessage(connectedUsername, " has left the chat");
    }
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
  if (clientSocket == -1) {
    throw std::runtime_error("Client socket is closed.");
  }
  return receive_message(clientSocket);
}

void ClientConnection::processMessage(MessageType type,
                                      const std::string &payload) {
  switch (type) {
  case MessageType::LOGIN:
    connectedUsername = payload;
    std::cout << "User logged in: \033[32m" << connectedUsername << "\033[0m"
              << '\n';
    {
      std::lock_guard<std::mutex> lock(server.clientsMutex);
      server.activeClients[connectedUsername] = clientSocket;
    }
    server.broadcastMessage(connectedUsername, "joined the chat");
    break;
  case MessageType::MESSAGE:
    std::cout << "Message from \033[32m" << connectedUsername
              << "\033[0m: " << payload << '\n';
    server.broadcastMessage(connectedUsername, payload);
    break;
  case MessageType::LOGOUT:
    std::cout << "User logged out: \033[32m" << payload << "\033[0m" << '\n';
    break;
  default:
    std::cerr << "\033[31mUnknown message type received.\033[0m" << '\n';
    break;
  }
}
