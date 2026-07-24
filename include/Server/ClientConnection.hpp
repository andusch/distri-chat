#pragma once

#include "Common/message.hpp"
#include <string>

class Server;

class ClientConnection {

public:
  ClientConnection(int clientSocket, Server &server);
  ~ClientConnection();

  void handleClient();
  void disconnect();

private:
  int clientSocket;
  bool connected;
  std::string connectedUsername;
  Server &server;

  void send(MessageType type, const std::string &payload);
  std::pair<MessageType, std::string> receive();
  void processMessage(MessageType type, const std::string &payload);
};
