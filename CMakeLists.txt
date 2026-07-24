cmake_minimum_required(VERSION 3.10)
project(DistriChat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(include)

add_executable(client 
    src/Client/main.cpp
    src/Client/Client.cpp
    src/Client/ConnectionHandler.cpp
)

target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server
    src/Server/main.cpp
    src/Server/Server.cpp
    src/Server/ClientConnection.cpp
    src/Server/DatabaseHandler.cpp
)
target_link_libraries(server PRIVATE Threads::Threads SQLite::SQLite3)
