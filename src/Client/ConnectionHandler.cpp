#include "Client/ConnectionHandler.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

ConnectionHandler::ConnectionHandler(const std::string &ip, short p)
    : clientSocket(-1), serverIp(ip), port(p) {}

ConnectionHandler::~ConnectionHandler() { disconnect(); }

void ConnectionHandler::connect() {
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    perror("socket failed");
    throw std::runtime_error("Failed to create socket");
  }

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);

  if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
    perror("Invalid address / address not supported");
    close(clientSocket);
    clientSocket = -1;
    throw std::runtime_error("Invalid server IP address");
  }

  std::cout << "\033[34mConnecting to server at " << serverIp << ":" << port
            << "...\033[0m" << '\n';
  std::this_thread::sleep_for(std::chrono::seconds(1));

  if (::connect(clientSocket, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) < 0) {
    perror("connection failed");
    close(clientSocket);
    clientSocket = -1;
    throw std::runtime_error("Failed to connect to server");
  }

  std::cout << "\033[34mConnected to server!\033[0m" << '\n';
}

void ConnectionHandler::disconnect() {
  if (clientSocket != -1) {
    close(clientSocket);
    clientSocket = -1;
    std::cout << "\033[34mDisconnected from server.\033[0m" << '\n';
  }
}

void ConnectionHandler::send(MessageType type, const std::string &payload) {
  if (clientSocket == -1) {
    throw std::runtime_error("Not connected to server.");
  }
  auto buffer = serialize_message(type, payload);
  ::send(clientSocket, buffer.data(), buffer.size(), 0);
}

std::pair<MessageType, std::string> ConnectionHandler::receive() {
  if (clientSocket == -1) {
    throw std::runtime_error("Not connected to server.");
  }
  return receive_message(clientSocket);
}
