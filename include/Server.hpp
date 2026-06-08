#pragma once

#include "ClientConnection.hpp"
#include "DatabaseHandler.hpp"
#include "message.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Server {

  friend class ClientConnection;

public:
  Server(short port);
  ~Server();

  void start();
  void stop();
  void broadcastMessage(const std::string &senderUsername,
                        const std::string &message);
  void addMessageToHistory(const std::string &username,
                           const std::string &message);
  std::vector<ChatMessage> getMessageHistory() const;

private:
  short port;
  int serverSocket;
  bool running;
  std::vector<std::thread> clientThreads;
  std::unique_ptr<DatabaseHandler> dbHandler;

  // Shared data with mutex protection
  std::map<std::string, int> activeClients;
  mutable std::mutex clientsMutex;
  std::vector<ChatMessage> messageHistory;
  mutable std::mutex historyMutex;

  void acceptConnections();
};
