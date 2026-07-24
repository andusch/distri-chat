#pragma once

#include "Common/message.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

class DatabaseHandler {

public:
  DatabaseHandler(const std::string &dbPath = "chat_history.db");
  ~DatabaseHandler();

  bool initialize();
  bool saveMessage(const std::string &username, const std::string &message);
  std::vector<ChatMessage> loadMessageHistory(int limit = 100);
  bool deleteDatabase();

private:
  sqlite3 *db;
  std::string dbPath;

  bool executeQuery(const std::string &query);
};
