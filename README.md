# ChatServer

## Introduction

This is a simple C++ chat server implemented as a simple exercise
to collaborate with my friend's JGQ to provide a fully operational
chat application using C++ as the back-end and Python as the front-end.

Link to JGQ client repository [here](https://github.com/JGQH/ChatClient).

## Requirements

To build this project you are required to have the following dependencies already installed in your machine:

* Any compiler with support for C++20
* CMake
* Boost.Asio
* nlohmann-json

*TODO: Add specific versions to each dependency.*

## Installation

Installation is fairly simple and straightforward:

```zsh
git clone https://github.com/Nikko-77/ChatServer.git
cd ChatServer
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
```

## Running

Simply run the program from the build directory:

```zsh
./ChatServer
```

## Milestones

* ~~Client able to connect to the Server.~~
* ~~Client able to send messages to the Server.~~
* ~~Server can send messages to the Client.~~
* ~~Server can execute commands requested by the Client~~
* ~~Server can send messages to multiple Clients.~~
* Send current users connected to the Client.
* Add additional commands for the Client to be able to request to the Server.

