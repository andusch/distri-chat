#pragma once

#include "ConnectionHandler.hpp"
#include <memory>
#include <string>

class Client {

public:
  Client(const std::string &serverIp, short port);
  void start();
  void stop();
  void login(const std::string &username);
  void sendMessage(const std::string &message);

private:
  std::unique_ptr<ConnectionHandler> connectionHandler;
  std::string username;
};
