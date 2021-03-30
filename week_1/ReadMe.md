# Chat Server-Client
## Purpose:
Create a Server-Client type Chat to communicate using c++ 11 programming language.

## File List
* **Server.cpp**: Contain all code implements a Server.
* **Client.cpp**: Contain all code implements a Client.

## Diagram of communication


## How to run?
* In terminal to access Server.cpp; run `g++ -std=c++11 -pthread Server.cpp -o Server.o`. Now Server is running.
* In other terminal to access Client.cpp; run `g++ -std=c++11 -pthread Client.cpp -o Client.o`. Now client is running.
* Write a message in Client, this message send so Server will print this message.
* Write a message in Server now; this message send to Client that it will print this message. 
