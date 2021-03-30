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

#define MAX_SIZE_BUFFER 256
#define PORT 45001

using namespace std;

string IP_CLIENT = "127.0.0.1";
char buffer[MAX_SIZE_BUFFER];

/**
 * Purpose:
 *  print message from server in client.
 * Params:
 * ip_conn (int) : id of connection.
 */
void printMsgByClient(int ip_conn){
    int n;
    bool flag = true;

    for(;;){
        bzero(buffer, MAX_SIZE_BUFFER);
        n = read(ip_conn, buffer,MAX_SIZE_BUFFER);
        cout << "read client" << endl;
        if (n < 0) perror("ERROR reading from socket");
        printf("the message is: [%s]\n", buffer);
        flag = true;
    }
    
    shutdown(ip_conn, SHUT_RDWR);
    
    close(ip_conn);
    return ;
}

/**
 * Purpose:
 *  send message from client to server.
 */
void typingIntoServer(int ip_conn){
    string msg;
    int n;

    for(;;){
        cout << ">>";
        getline(cin, msg);
        n = write(ip_conn, msg.c_str(), MAX_SIZE_BUFFER);
        if (n < 0) perror("ERROR writing to socket");
        msg = "";
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
    ///n = write(SocketFD,"Hi, this is Peter.",18);
    string msg;
    /* perform read write operations ... */
    thread ClientSender(typingIntoServer, SocketFD);
    ClientSender.join();

    for(;;){
        bzero(buffer, MAX_SIZE_BUFFER);
        n = read(SocketFD, buffer, MAX_SIZE_BUFFER);
        cout << "read client" << endl;
        if (n < 0) perror("ERROR reading from socket");
        printf("Here is the message: [%s]\n", buffer);
        ///cout << ">>";
        ///getline(cin, msg);
        ///n = write(SocketFD, msg.c_str(), MAX_SIZE_BUFFER);
        ///msg = "";
    }


    ///thread ClientListener(printMsgByClient, SocketFD);
    ///ClientListener.join();
    ///shutdown(SocketFD, SHUT_RDWR);
 
    ///close(SocketFD);
    ///return 0;
}
