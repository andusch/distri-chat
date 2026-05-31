cmake_minimum_required(VERSION 3.10)
project(DistriChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include)

target_link_libraries(server PRIVATE pthread)
target_link_libraries(client PRIVATE pthread)
