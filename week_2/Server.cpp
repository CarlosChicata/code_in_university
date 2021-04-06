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

#include <typeinfo>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include "utils_socket.h"

#define MAX_SIZE_BUFFER 256
#define PORT 45000

using namespace std;

class Client;
vector<Client* > connections;
char buffer[MAX_SIZE_BUFFER];

//// UTILITY METHODS

//// CORE METHODS

/**
 * Object to manage connection of client.
 */
class Client{
  private:
    int idConnection; ///id of connection
    string name; /// name of client
    bool isActive = true; /// manage loop to interactive with server
    string internalBuffer; //// buffer of connection
  public:
    /**
     * Purpose:
     *  Preparing context of manager connection.
     */
    Client(int id): idConnection(id){
      connections.push_back(this);
      this->internalBuffer = string(MAX_SIZE_BUFFER, '\0');
    }

    /**
     * Purpose:
     *  interactive with other clients using message.
     */
    void interaction(){
      /// interactive with server
      while(this->isActive){
        readSocket(this->idConnection, (char*)this->internalBuffer.c_str(), MAX_SIZE_BUFFER);
        manageCommand(this->internalBuffer);
        this->internalBuffer = string(MAX_SIZE_BUFFER, '\0');
      }

      shutdown(this->idConnection, SHUT_RDWR);
      close(this->idConnection);
    }

    /**
     * Purpose:
     *  process command client inside message
     * Params:
     *  - command (string): message of client
     * Return depending of command.
     */
    void manageCommand(string command){
      cout << command << endl;
      pair<string, string> processedCommand =  segmentationString(command, " ");
      string token = processedCommand.first;

      /// control of options
      if(token == "name"){
        this->name = processedCommand.second;
        writeSocket(this->idConnection, "ok", MAX_SIZE_BUFFER);
      }else if(token == "msg"){
        //// msg to client
        processedCommand = segmentationString( processedCommand.second, " ");
        for(Client* c: connections){;
          if(comparingString(c->name, processedCommand.first)){
            writeSocket(c->idConnection, processedCommand.second.c_str(), MAX_SIZE_BUFFER);
          }
        }
      }else if(comparingString(processedCommand.second, "exit")){
        cout << "entramos a la salida" << endl;
        //// close connection
        writeSocket(this->idConnection, "exitcon", MAX_SIZE_BUFFER);
        for(vector<Client* >::iterator it = connections.begin(); it != connections.end(); it++){
          if((*it)->idConnection == this->idConnection){
            connections.erase(it);
            break;
          }
        }
        this->isActive = false;
      }else{

      }
    }

    /**
     * Purpose:
     *  clean buffer of client.
     */
    void cleanBuffer(){
      this->internalBuffer = string(MAX_SIZE_BUFFER, '\0');
    }
};

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
        
        Client* temp = new Client(ConnectFD);
        thread th(&Client::interaction, temp);
        th.detach();
    }
 
    close(SocketFD);
    return 0;
  }
