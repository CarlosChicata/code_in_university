/**
Purpose:
    run server program to testing socket
How to run:
    g++ -std=c++11 -pthread Server.cpp -o server.o
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
#include <vector>

#define MAX_SIZE_BUFFER 256
#define PORT 45001

using namespace std;

vector<int> conn_list;
char buffer[MAX_SIZE_BUFFER];


int main(void)
  {
    struct sockaddr_in stSockAddr, cliSockAddr;
    int SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    int n;
    string msgTemp = "";

    /// is socket exist?
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    memset(&cliSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(PORT);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    /// is socket binding?
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    int lenCli = sizeof(cliSockAddr);

    for(;;){
      n = recvfrom(SocketFD, 
        buffer, 
        MAX_SIZE_BUFFER, 
        MSG_WAITALL, 
        (struct sockaddr *) &cliSockAddr, 
        (socklen_t *) &lenCli );

      buffer[n] = '\0';

      printf("\nClient : %s\n", buffer);
      memset(buffer, '\0', MAX_SIZE_BUFFER);
      msgTemp = "";

      cout << ">> ";
      getline(cin , msgTemp);

      sendto(
        SocketFD, 
        (const char *)msgTemp.c_str(),
        msgTemp.size(), 
        MSG_CONFIRM, 
        (const struct sockaddr *) &cliSockAddr,
        lenCli);
    }

    close(SocketFD);
    return 0;
  }

