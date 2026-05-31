#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "message.hpp"

const short PORT = 8080;
const char *SERVER_IP = "127.0.0.1"; // loop-back address

void send_message(int sock, MessageType type, const std::string &payload) {
  auto buffer = serialize_message(type, payload);
  send(sock, buffer.data(), buffer.size(), 0);
}

int main() {

  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    perror("socket failed");
    return -1;
  }

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);

  if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0) {
    perror("Invalid address / Address not supported");
    close(clientSocket);
    return -1;
  }

  std::cout << "Connecting to server at " << SERVER_IP << ":" << PORT << "..."
            << '\n';

  sleep(1);

  if (connect(clientSocket, (struct sockaddr *)&serverAddress,
              sizeof(serverAddress)) < 0) {
    perror("connection failed");
    close(clientSocket);
    return -1;
  }

  std::cout << "Connected to server!" << '\n';

  // Login
  std::string username;
  std::cout << "Enter username: ";
  std::getline(std::cin, username);
  send_message(clientSocket, LOGIN, username);

  try {
    auto [type, payload] = receive_message(clientSocket);
    std::cout << "Server: " << payload << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    close(clientSocket);
    return -1;
  }

  std::string message;
  std::cout << "Enter message: ";
  std::getline(std::cin, message);
  send_message(clientSocket, MESSAGE, message);

  try {
    auto [type, payload] = receive_message(clientSocket);
    std::cout << "Server: " << payload << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    close(clientSocket);
    return -1;
  }

  send_message(clientSocket, LOGOUT, username);
  close(clientSocket);
  return 0;
}
