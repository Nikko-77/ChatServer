cmake_minimum_required(VERSION 3.15)

project(ChatServer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED thread system)
find_package(nlohmann_json REQUIRED)

add_executable(${PROJECT_NAME} "chat_server.cc")
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread nlohmann_json::nlohmann_json)
