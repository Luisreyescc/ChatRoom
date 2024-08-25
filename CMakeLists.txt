cmake_minimum_required(VERSION 3.10)

project(ChatClient)

set(CMAKE_C_STANDARD 99)

add_executable(client src/client/main.c src/client/client.c)
