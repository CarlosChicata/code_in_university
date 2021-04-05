/**
Purpose:
    run client program to testing socket
How to run:
    g++ -std=c++11 Client.cpp -pthread -o client.o
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include "utils_socket.h"

#define MAX_SIZE_BUFFER 256
#define PORT 45002

using namespace std;

string IP_CLIENT = "127.0.0.1";
char buffer[MAX_SIZE_BUFFER];
bool isActive = true;

void sendMsg(int id_conn){
  string internalBuffer(MAX_SIZE_BUFFER, '\0');

  while(isActive){
    printf(">>");
    getline(cin, internalBuffer);
    writeSocket(id_conn, internalBuffer.c_str(), MAX_SIZE_BUFFER);
    internalBuffer = string(MAX_SIZE_BUFFER, '\0');
  }

}

void receiveMsg(int id_conn){
  string internalBuffer;
  
  while(isActive){
    readSocket(id_conn, (char*) internalBuffer.c_str(), MAX_SIZE_BUFFER);
    printf("\b\b%s\n>>", internalBuffer.c_str());
    fflush(stdout);
  }
}


int main(){
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;

 
    /// is socket created?
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(PORT);
    Res = inet_pton(AF_INET, IP_CLIENT.c_str(), &stSockAddr.sin_addr);
 
    /// is params ok?
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    /// is connection ok?
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    /* perform read write operations ... */
    thread ClientSender(sendMsg, SocketFD);
    thread ClientListener(receiveMsg, SocketFD);
    ClientSender.detach();
    ClientListener.detach();
    
    for(;;){
    }

    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
    return 0;
}