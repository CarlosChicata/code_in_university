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


/**
 * Purpose:
 *  process write-read server from client.
 * 
 * Params:
 *  - ip_con (int): ip of connection.
 */
void processThread(int ip_conn){
    int n;
    bool flag = true;
    string msg;

    while(flag){
        /// read process
        bzero(buffer, MAX_SIZE_BUFFER);
        n = read(ip_conn, buffer,MAX_SIZE_BUFFER);
        if (n < 0) perror("ERROR reading from socket");
        printf("Here is the message: [%s]\n", buffer);
        /// write process
        cout << ">>";
        getline(cin, msg);
        n = write(ip_conn, msg.c_str(), MAX_SIZE_BUFFER);
        if (n < 0) perror("ERROR writing to socket");
        flag = true;
    }
    
    shutdown(ip_conn, SHUT_RDWR);
    
    close(ip_conn);
    return ;
}


int main(void)
  {
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;

    /// is socket exist?
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
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
 
    /// is socket listenning?
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    /// manage connection into server
    for(;;)
    {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        /// is connection accepted?
        if(0 > ConnectFD)
        {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        /// store connect in general list
        conn_list.push_back(ConnectFD);

        /// create a listener to message from client
        thread clientListener(processThread, ConnectFD);
        clientListener.join();
    }
 
    close(SocketFD);
    return 0;
  }

