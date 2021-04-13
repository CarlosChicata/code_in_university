// Oscar Daniel Ramos Ramirez
/**
 * Purpose:
 *  implement of server to multichat with custom protocol.
 * 
 * Limitations:
 *  - can access duplicated name between clients.
 * 
 * How To run:
 *  1.- generate executor using `g++ -std=c++11 -pthread server.cpp -o server.o`
 *  2.- run executor and passing port to listen: ``serve.o <port number>`
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
#include <thread>
#include <signal.h>
#include "stringhelper.h"

using namespace std;

//////////////////////////////AUX/////////////////////////////
///// define a head methods that next implementations will use

int ___socket(int domain, int type, int protocol);
int ___inet_pton(int af, const char* src, void* dst);
void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen);
void ___bind(int sockFD, struct sockaddr* my_addr, int addrlen);
void ___listen(int sockFD, int backlog);
int ___accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen);
void ___read(int fd, char buf[], int len);
void ___write(int fd, const char* msg, int len);

////////////////////////////THREADS///////////////////////////////////
class Client;
vector<Client*> clients; //// store active connection inside server

/**
 * Purpose:
 *  manage connection of client like a independent and separate object
 * 
 */
class Client
{
private:
    int connectFD; /// id of connection
    string nickname = ""; //// nick name of client
    string lastResponse; // Para msg (?)
    bool isActive = true; /// manage status of connection inside server

public:
    Client(int connectFD): connectFD(connectFD){
        clients.push_back(this);
    }

    /**
     * Purpose:
     *  manage msg inside connection of client
     */
    void receivePackages(){
        do{
            string pack(1000, '\0'); /// clean buffer
            ___read(connectFD, (char*)pack.c_str(), 1000); /// read message of client
            
            ////cout << "sebser" << pack << endl;
            if(isRequest(pack)){ /// check if msg is command
                pack = request2response(pack); /// process message of client
                ___write(connectFD, pack.c_str(), pack.size()); /// alway responde of command

            }else if(isRequestBySanti(pack)){
                pack = request2response(pack);
            }
            else{
                ////cout << "c" << endl;
                lastResponse = response2string(pack); /// generate response of string (this is useless part of server)
            }
        }while(this->isActive);

        // Cerrar conexion
        shutdown(connectFD, SHUT_RDWR);
        close(connectFD);
    }

    /**
     * Purpose:
     *  process msg and generate a responde of msg to client.
     * Params:
     *  - pack: msg of client to process and responde it.
     * Return a response will send to client.
     * NOTE: 
     *  - This is a receive manager. This part interprete msg of client.
     */
    string request2response(string pack)
    {
        string response; /// temp buffer
        int idx = 2;
        /////cout << pack << endl;
        ////cout << "pradckat: "<< pack[0] << endl;
        switch(pack[0]){
        case '1': // nick command
        {
            string nick = parserGetField(pack, idx, 2);
            this->nickname = nick;
            response = join({"5"}); // OK
            break;
        }
        case '2': // list command
        {
            vector<string> list;
            for(Client* c: clients) if(!c->nickname.empty())
                list.push_back(c->nickname);
            string listmsg = split(list, ", "); // "usr1, usr2, ..."
            response = join({"6", len(listmsg, 3), listmsg});
            break;
        }
        case '3': // msg command
        {
            string tocli, msg;
            tocli = parserGetField(pack, idx, 2); /// get field from msg
            msg = parserGetField(pack, idx, 3); /// get field from msg
            for(Client* c: clients) if(c->nickname == tocli)
            {
                sendMsgToDestinataryClient(c, msg); //// send msg to client
                if(destinatarySendedOk(c)) /// check if destiny client receive correct the msg sent
                    response = join({"5"}); // ok
                else
                    response = errorMsg("Error en cliente receptor"); // error
            }
            if(response.empty()) // check if client receive answer
               response = errorMsg("Cliente no encontrado"); // error
            break;
        }
        case 'i': { /// new command: list by santi protocol
            string tokenizeRptSize;
            string tokenizeRptName = "";
            bool isEmpty = false;

            for(int i=0; i < clients.size(); ++i){
                if(clients[i]->nickname == ""){
                    isEmpty = true;
                    break;
                }
                tokenizeRptSize += len(clients[i]->nickname, 2);
                tokenizeRptName += clients[i]->nickname;
            }
            
            if(isEmpty){
                response = joinBySanti({"IE", " Wrong names in list" }); /// error
            }else{
                response = joinBySanti({"I", fillZero(to_string(clients.size()), 2), tokenizeRptSize + tokenizeRptName}); // ok
            }

            break;
        }
        case 'l': { /// new command: login command by santi protocol
            int idxCommand = 1;
            int sizeUser = parserGetFieldByInt(pack, idxCommand, 2);
            int sizePass = parserGetFieldByInt(pack, idxCommand, 2);
            string valueUser = parserGetFieldByString(pack, idxCommand, sizeUser);
            string valuePass = parserGetFieldByString(pack, idxCommand, sizePass);

            if(valuePass == "ucsp"){
                this->nickname = valueUser;
                response = joinBySanti({"L", "ok" }); /// ok
            }else{
                response = joinBySanti({"E", " Invalided pass!!!!!" }); /// error
            }
            break;
        }
        case 'm': {
            int idxCommand = 1;
            int sizeMsg = parserGetFieldByInt(pack, idxCommand, 3);
            int sizeUser = parserGetFieldByInt(pack, idxCommand, 2);
            string valueMsg = parserGetFieldByString(pack, idxCommand, sizeMsg);
            string valueUser = parserGetFieldByString(pack, idxCommand, sizeUser);
            string rpta = "";
            bool isSend = true;

            for(int i=0; i < clients.size(); ++i){
                if(clients[i]->nickname == valueUser){
                    rpta = joinBySanti({"M", len(valueMsg, 3), len( this->nickname,2) , valueMsg, this->nickname});
                    ___write(clients[i]->connectFD, rpta.c_str(), rpta.size());
                    pack = joinBySanti({"L", "ok" });;
                    isSend = false;
                    break;
                }
            }

            if(isSend == true){
                response = joinBySanti({"E", " unallow name server" }); /// error
            }
            break;
        }
        case 'b': {
            int idxCommand = 1;
            string rpta = "";
            int sizeMsg = parserGetFieldByInt(pack, idxCommand, 3);
            string valueMsg = parserGetFieldByString(pack, idxCommand, sizeMsg);
            bool sentAny = false;
            
            for(int i = 0; i < clients.size() ; i++){
                if(clients[i] != this){
                    sentAny = true;
                    rpta = joinBySanti({"B", len(valueMsg, 3), len(clients[i]->nickname, 2), valueMsg, clients[i]->nickname });
                    ___write(clients[i]->connectFD, rpta.c_str(), rpta.size());
                }
            }

            if(sentAny == false){
                response = joinBySanti({"E", " unallow clients!!!!" }); /// error
            }
            break;
        }
        case 'x': {
            int idxCommand = 1;
            string rpta = parserGetFieldByString(pack, idxCommand, 255);
            ////cout << rpta << endl;
            rpta = joinBySanti({"X", rpta});
            ////cout << "eptra" << rpta << endl;
            for(int i = 0; i < clients.size() ; i++){
                ////cout << "enviamos!" << endl;
                ___write(clients[i]->connectFD, rpta.c_str(), rpta.size());
            }
        }
        case '8': // exit command
            for(int i=0; i<clients.size(); ++i) if(clients[i] == this)
                clients.erase(clients.begin() + i); /// remove from client store inside server.
            this->isActive = false; /// close connection
            response = join({"5"}); // ok
            break;
        default: /// send wrong command to client
            response = errorMsg("Comando no identificado");
        }

        ///if(this->nickname.empty() && pack[0] != '8') /// check if connection has a nickname
        ///    response = errorMsg("Necesitas un nickname!!"); 
        return response;
    }

    /**
     * Purpose:
     *  get status of command from client.
     * Params:
     *  - pack: msg to process.
     * return a status of msg.
     */
    string response2string(string pack){
        string str;
        switch(pack[0])
        {
            case '4': // error
                str = errorMsg("Error en cliente receptor");
            break;
            case '5': // OK
                str = join({"5"});
            break;
        }
        return str;
    }

    /**
     * Purpose:
     *  send msg to client
     * Params:
     *  - c : client to receive msg.
     *  - msg: msg to send.
     * Return None.
     */
    void sendMsgToDestinataryClient(Client* c, string msg){
        string pack;
        pack = join({"7", len(this->nickname, 2), this->nickname, len(msg, 3), msg});
        // printf("Enviando a destinatario [%s]\n", pack.c_str());
        ___write(c->connectFD, pack.c_str(), pack.size()); 
    }

    /**
     * Purpose:
     *  check if last response is ok or not.
     * Params:
     *  - c : client to update field.
     * return boolean.
     */
    bool destinatarySendedOk(Client* c){
        sleep(1); // Lo esperamos solo 1 segundo
        return c->lastResponse == join({"5"}); //OK
    }
    
    /**
     * Purpose:
     *  check if this msg to responde a command.
     * Params:
     *  pack: msg from client.
     */ 
    bool isRequest(string pack){
        return pack[0] == 'm' || pack[0] == 'i' || pack[0] == 'l' || pack[0] == '1' || pack[0] == '2' || pack[0] == '3' || pack[0] == '8';
    }

    /// same 'isRequest' method for santi protocols
    bool isRequestBySanti(string pack){
        return pack[0] == 'x' || pack[0] == 'm' || pack[0] == 'b';
    }
};

//// contain a main code to run server
int server_Thread(int serverPort){
    try{
        // Crear socket
        int sockFD;
        struct sockaddr_in stSockAddr;  
        sockFD = ___socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
         
        // Crear peer server
        /// seteando variables servidor(PORT, IP AUTOMATICO)
        memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(serverPort);
        stSockAddr.sin_addr.s_addr = INADDR_ANY; 

        ___bind(sockFD, (struct sockaddr*)&stSockAddr, sizeof(struct sockaddr_in));
        ___listen(sockFD, 10);
        printf("Servidor abierto\n");

        // Aceptar conexiones entrantes
        while(true){
            int connectFD;
            connectFD = ___accept(sockFD, NULL, NULL);
            printf("cliente conexion recibida\n");
            Client* c = new Client(connectFD); /// create a client conection manager object
            thread th(&Client::receivePackages, c); /// link this manager with thread
            th.detach(); /// separate thread from main thread of executor
        }
        // Cierro el servidor
        close(sockFD);
    }
    catch(string msg_error){
        perror(msg_error.c_str()); // perror da mas detalle que printf 
        exit(EXIT_FAILURE);
    }
}
//////////////////////////////SIGNAL//////////////////////////////

//// exit executor if something go wrong
void sighandler(int signum){
    exit(0); // Mas seguro segun vi
}

//////////////////////////////MAIN////////////////////////////////
// >>hostname -I: saca mi IP, en consola
int main(int argc, const char** argv){
    signal(SIGKILL, sighandler);
    if(argc != 2)
    {
        printf("Modelo : ./server.exe <Puerto del server>\n");
        printf("Ejemplo: ./server.exe 1480\n");
        exit(EXIT_FAILURE);
    }
    int serverPort = atoi(argv[1]); //// get a Port value
    server_Thread(serverPort); //// main code to run in server
    return 0;
}




//////////////////////////////AUX/////////////////////////////
//// implementations of head methods

//// create a socket
int ___socket(int domain, int type, int protocol){
    int sockFD = socket(domain, type, protocol);
    if (-1 == sockFD)
      throw string("cannot create socket");
    return sockFD;
}

int ___inet_pton(int af, const char* src, void* dst)
{
    int res = inet_pton(af, src, dst);
    if (0 > res)
      throw string("error: first parameter is not a valid address family");
    else if (0 == res)
      throw string("char string (second parameter does not contain valid ipaddress");
    return res;
}

//// generate a id of connection
void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen){
    int res = connect(sockFD, serv_addr, addrlen);
    if(res == -1)
      throw string("connect failed");
}

//// link id connection with port & IP
void ___bind(int sockFD, struct sockaddr* my_addr, int addrlen){
    int res = bind(sockFD, my_addr, addrlen);
    if(res == -1)
      throw string("error bind failed");
}

//// enable listen connection
void ___listen(int sockFD, int backlog){
    int res = listen(sockFD, backlog);
    if(res == -1)
      throw string("error listen failed");
}

//// accept connection of client inside server
int ___accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen){
    int connectFD = accept(sockFD, addr, addrlen);
    if(connectFD < 0)
      throw string("error accept failed");
    return connectFD;
}

//// read msg of client
void ___read(int fd, char buf[], int len){
    int n = read(fd, buf, len);
    if(n < 0) 
      throw string("ERROR reading from socket");
}   

//// write msg to client
void ___write(int fd, const char* msg, int len){
    int n = write(fd, msg, len);
    if(n < 0)
      throw string("ERROR writing to socket");
}

