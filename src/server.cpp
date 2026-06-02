#include "../include/Server.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(short p) : port(p), serverSocket(-1), running(false) {}
Server::~Server() { stop(); }

void Server::start() {

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    perror("socket failed");
    throw std::runtime_error("Failed to create server socket.");
  }

  int opt = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) < 0) {
    perror("bind failed");
    close(serverSocket);
    serverSocket = -1;
    throw std::runtime_error("Failed to bind server socket");
  }

  if (listen(serverSocket, 5) < 0) {
    perror("listen failed");
    close(serverSocket);
    serverSocket = -1;
    throw std::runtime_error("Failed to listen on server socket");
  }

  std::cout << "Chat server listening on port: " << port << "\n";
  running = true;
  acceptConnections();
}

void Server::stop() {
  running = false;
  if (serverSocket != -1) {
    close(serverSocket);
    serverSocket = -1;
  }
  for (std::thread &t : clientThreads) {
    if (t.joinable()) {
      t.join();
    }
  }
  std::cout << "Server stopped.\n";
}

void Server::acceptConnections() {
  while (running) {
    sockaddr_in clientAddress{};
    socklen_t clientLen = sizeof(clientAddress);
    int clientSock =
        accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLen);
    if (clientSock < 0) {
      if (running) {
        perror("accept failed");
      }
      continue;
    }
    std::cout << "Client connected!\n";
    clientThreads.emplace_back([clientSock]() {
      ClientConnection clientConn(clientSock);
      clientConn.handleClient();
    });
  }
}
