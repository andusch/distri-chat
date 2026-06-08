#include "../include/DatabaseHandler.hpp"
#include <algorithm>
#include <iostream>

DatabaseHandler::DatabaseHandler(const std::string &dbPath)
    : db(nullptr), dbPath(dbPath) {}

DatabaseHandler::~DatabaseHandler() {
  if (db)
    sqlite3_close(db);
}

bool DatabaseHandler::initialize() {
  if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << '\n';
    return false;
  }

  const char *createTableQuery = "CREATE TABLE IF NOT EXISTS messages ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "username TEXT NOT NULL,"
                                 "content TEXT NOT NULL,"
                                 "timestamp TEXT NOT NULL"
                                 ");";

  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, createTableQuery, nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL Error: " << errMsg << '\n';
    sqlite3_free(errMsg);
    return false;
  }

  std::cout << "Database initialized successfully." << '\n';
  return true;
}

std::string escapeSql(const std::string &input) {
  std::string output;
  for (char c : input) {
    if (c == '\'')
      output += "\\'";
    else
      output += c;
  }
  return output;
}

bool DatabaseHandler::saveMessage(const std::string &username,
                                  const std::string &message) {
  std::string timestamp = getCurrentTimestamp();
  std::string escapedUsername = escapeSql(username);
  std::string escapedMessage = escapeSql(message);
  std::string query =
      "INSERT INTO messages (username, content, timestamp) VALUES ('" +
      escapedUsername + "', '" + escapedMessage + "', '" + timestamp + "');";

  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL Error: " << errMsg << '\n';
    sqlite3_free(errMsg);
    return false;
  }

  return true;
}

std::vector<ChatMessage> DatabaseHandler::loadMessageHistory(int limit) {
  std::vector<ChatMessage> messages;
  sqlite3_stmt *stmt;

  std::string query = "SELECT username, content, timestamp FROM messages ORDER "
                      "BY id DESC LIMIT ?";

  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
              << std::endl;
    return messages;
  }

  sqlite3_bind_int(stmt, 1, limit);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *username_char =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    const char *content_char =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    const char *timestamp_char =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    std::string username = username_char ? username_char : "";
    std::string content = content_char ? content_char : "";
    std::string timestamp = timestamp_char ? timestamp_char : "";

    messages.emplace_back(username, content, timestamp);
  }

  sqlite3_finalize(stmt);
  std::reverse(messages.begin(), messages.end());
  return messages;
}
