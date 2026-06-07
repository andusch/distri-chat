#pragma once

#include "ClientConnection.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Server {

public:
  std::map<std::string, int> activeClients;
  std::mutex clientsMutex;
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

public:
  void broadcastMessage(const std::string &senderUsername,
                        const std::string &message);
};
