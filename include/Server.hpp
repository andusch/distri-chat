#pragma once

#include "ClientConnection.hpp"
#include <memory>
#include <string>
#include <thread>
#include <vector>

class Server {

public:
  Server(short port);
  ~Server();

  void start();
  void stop();

private:
  short port;
  int serverSocket;
  bool running;
  std::vector<std::thread> clientThreads;

  void acceptConnections();
};
