cmake_minimum_required(VERSION 3.10)
project(DistriChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(client client.cpp src/ConnectionHandler.cpp src/Client.cpp)
add_executable(server server.cpp src/ClientConnection.cpp src/Server.cpp)

target_link_libraries(client pthread)
target_link_libraries(server pthread)
