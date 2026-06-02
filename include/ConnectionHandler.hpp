#pragma once

#include "message.hpp"
#include <string>
#include <utility>

class ConnectionHandler {

public:
  ConnectionHandler(const std::string &serverIp, short port);
  ~ConnectionHandler();

  void connect();
  void disconnect();
  void send(MessageType type, const std::string &payload);
  std::pair<MessageType, std::string> receive();
  bool isConnected() const { return clientSocket != -1; }

private:
  int clientSocket;
  std::string serverIp;
  short port;
};
