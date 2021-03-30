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
