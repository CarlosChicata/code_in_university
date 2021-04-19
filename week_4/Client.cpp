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
 *  process write-client from client to server.
 * Params:
 * ip_conn (int) : id of connection
 */
void procesThread(int ip_conn){
    string msg;
    int n;

    for(;;){
        /// write process
        cout << ">>";
        getline(cin, msg);
        n = write(ip_conn, msg.c_str(), MAX_SIZE_BUFFER);
        if (n < 0) perror("ERROR writing to socket");
        /// read process
        msg = "";
        bzero(buffer, MAX_SIZE_BUFFER);
        n = read(ip_conn, buffer,MAX_SIZE_BUFFER);
        cout << "read client" << endl;
        if (n < 0) perror("ERROR reading from socket");
        printf("the message is: [%s]\n", buffer);
    }
}


int main(){
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    int n;
    string msgTemp = "";
 
    /// is socket created?
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(PORT);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    for(;;){
      cout << ">> ";
      getline(cin , msgTemp);

      sendto(SocketFD,
        msgTemp.c_str(),
        msgTemp.size(),
        MSG_CONFIRM,
        (const struct sockaddr *) &stSockAddr,
        sizeof(stSockAddr));

      Res = sizeof(stSockAddr);

      n = recvfrom(
        SocketFD,
        buffer,
        MAX_SIZE_BUFFER,
        MSG_WAITALL,
        (struct sockaddr *) &stSockAddr,
        (socklen_t *) &Res);

      buffer[n]= '\0';
      printf("\nServer: %s\n", buffer);
      memset(buffer, '\0', MAX_SIZE_BUFFER);
      msgTemp = "";
    }
    close(SocketFD);
    return 0;
}
