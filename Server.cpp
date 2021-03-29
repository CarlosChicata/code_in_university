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
 *  print message from client.
 * 
 * Params:
 *  - ip_con (int): ip of connection.
 */
void printMsgByClient(int ip_conn){
    int n;
    bool flag = true;

    while(flag){
        bzero(buffer, MAX_SIZE_BUFFER);
        n = read(ip_conn, buffer,MAX_SIZE_BUFFER);
        cout << "read" << endl;
        if (n < 0) perror("ERROR reading from socket");
        cout << "from thread" << endl;
        printf("Here is the message: [%s]\n", buffer);
        n = write(ip_conn, "hola desde el server!", MAX_SIZE_BUFFER);
        if (n < 0) perror("ERROR writing to socket");
        flag = true;
    }
    
    shutdown(ip_conn, SHUT_RDWR);
    
    close(ip_conn);
    return ;
}

/**
 * Purpose:
 *  send message all client connect.
 */
void typingFromServer(){
    string msg;
    int n;

    while(true){
        cout << ">>";
                getline(cin, msg);
        cout << msg;
        ///for(int index = 0; index < conn_list.size(); index++){
        ///    n = write(conn_list[index], msg.c_str(), MAX_SIZE_BUFFER);
        ///    if (n < 0) perror("ERROR writing to socket");
        ///}
    }

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

    ///thread serverTrigger(typingFromServer);
    //serverTrigger.join();
 
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
        thread clientListener(printMsgByClient, ConnectFD);
        clientListener.join();
        string msg;
        for(;;){
            cout << ">>";
            getline(cin, msg);
            n = write(ConnectFD, msg.c_str(), MAX_SIZE_BUFFER);
            if (n < 0) perror("ERROR writing to socket");
        }
        ///bzero(buffer,MAX_SIZE_BUFFER);
        ///n = read(ConnectFD,buffer,MAX_SIZE_BUFFER);
        ///if (n < 0) perror("ERROR reading from socket");
        ///printf("Here is the message: [%s]\n",buffer);
        ///n = write(ConnectFD,"I got your message",18);
        ///if (n < 0) perror("ERROR writing to socket");
    
        /* perform read write operations ... */
    }
 
    close(SocketFD);
    return 0;
  }
