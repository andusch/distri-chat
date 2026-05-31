#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "message.hpp"

const short PORT = 8080;

void send_message(int sock, MessageType type, const std::string &payload) {
  auto buffer = serialize_message(type, payload);
  send(sock, buffer.data(), buffer.size(), 0);
}

int main() {

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (serverSocket < 0) {
    perror("socket failed");
    close(serverSocket);
    return -1;
  }

  int opt = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) < 0) {
    perror("bind failed");
    close(serverSocket);
    return -1;
  }

  if (listen(serverSocket, 5) < 0) {
    perror("listen failed");
    close(serverSocket);
    return -1;
  }

  std::cout << "Chat server listening on port: " << PORT << '\n';

  int clientSocket = accept(serverSocket, nullptr, nullptr);
  if (clientSocket < 0) {
    perror("accept failed");
    close(serverSocket);
    return -1;
  }

  std::cout << "Client connected!" << '\n';

  while (true) {
    // Read header first
    uint8_t header_buffer[sizeof(MessageHeader)];
    ssize_t bytes_received =
        recv(clientSocket, header_buffer, sizeof(MessageHeader), MSG_WAITALL);

    if (bytes_received <= 0) {
      break;
    }; // Client disconnected or error

    // Parse header to get payload length
    const MessageHeader *header =
        reinterpret_cast<const MessageHeader *>(header_buffer);
    uint32_t payload_length = ntohl(header->length);

    // Read payload
    std::vector<uint8_t> payload_buffer(payload_length);
    bytes_received =
        recv(clientSocket, payload_buffer.data(), payload_length, MSG_WAITALL);
    if (bytes_received != payload_length) {
      std::cerr << "Incomplete payload received!" << '\n';
      break;
    }

    // Deserialize
    try {

      MessageType type = static_cast<MessageType>(header->type);
      std::string payload(reinterpret_cast<const char *>(payload_buffer.data()),
                          payload_length);

      switch (type) {
      case LOGIN:
        std::cout << "User logged in: " << payload << '\n';
        send_message(clientSocket, MESSAGE, "Welcome, " + payload + "!");
        break;
      case MESSAGE:
        std::cout << "Message: " << payload << '\n';
        send_message(clientSocket, MESSAGE, payload);
        break;
      case LOGOUT:
        std::cout << "User logged out: " << payload << '\n';
        close(clientSocket);
        return 0;
      }
    } catch (const std::exception &e) {
      std::cerr << "Deserialization error: " << e.what() << '\n';
      break;
    }
  }

  close(clientSocket);
  close(serverSocket);

  return 0;
}
