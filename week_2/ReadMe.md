# Chat multi-client

## Purpose
Create a chat for several clients. The manage connection between clients is responsibility of server.

## File List
* **Client.cpp**: Contain all code implements for Client.
* **Server.cpp**: Contain all code implements for Server.
* **utils_socket.h**: Contain all code implements to support communication socket and manage the message. 

## Diagram of project

Soon on.

## How to run
### General to run server/client
* In terminal to access Server.cpp; run `g++ -std=c++11 -pthread Server.cpp -o Server.o`. Now Server is running.
* In other terminal to access Client.cpp; run `g++ -std=c++11 -pthread Client.cpp -o Client.o`. Now client is running.

### For Client.
* first you register your nickname using `name` command. So this part to talk with other clients.
* Send message to other connected client in chat using `msg` command.
* Exit this chat using `exit` command.

## Available commands 
Those are command that clients can use in chat.

### name command
Use to name client in chat; it's not validate it this duplicate in chat.
This is structure: `name <name of client>` with:
  * `name of client`: name of client wants to use.

### msg command
Use to send a message to other or same client in chat.
This is structure: `msg <name of client> <message>` with:
  * `name of client`: name of client you want to talk.
  * `message`: message to send a client.

### exit command
Use to close participation in chat.

## Limitations list
Every mark in list if solved limitation.
- [ ] The IP and Port area hard coding in files or server and client. I need to passing those params by terminal.
- [ ] Hard change structure of manage command by protocols structure. I need to separate manage from implementation of protocols by add middleware layers.
- [ ] Not have validation of duplicate of nickname.  Solved [ ]
