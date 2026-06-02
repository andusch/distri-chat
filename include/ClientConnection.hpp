#pragma once

#include "message.hpp"
#include <string>

class ClientConnection {

public:
  ClientConnection(int clientSocket);
  ~ClientConnection();

  void handleClient();

private:
  int clientSocket;
  std::string connectedUsername;

  void send(MessageType type, const std::string &payload);
  std::pair<MessageType, std::string> receive();
  void processMessage(MessageType type, const std::string &payload);
};
