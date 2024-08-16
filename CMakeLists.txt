cmake_minimum_required(VERSION 3.16)

project(Data_Server)

include_directories(server include/)

add_executable(server src/main.cpp)