 /* Client code in C */
// Oscar Daniel Ramos Ramirez
 
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
#include <vector>
#include <initializer_list>
#include "stringhelper.h"

using namespace std;

//////////////////////////////AUX/////////////////////////////
// Agrega una capa de seguridad a cada funcion

int ___socket(int domain, int type, int protocol);
int ___inet_pton(int af, const char* src, void* dst);
void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen);
void ___bind(int sockFD, struct sockaddr* my_addr, int addrlen);
void ___listen(int sockFD, int backlog);
int ___accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen);
void ___read(int fd, char buf[], int len);
void ___write(int fd, const char* msg, int len);

////////////////////////////VARS/////////////////////////////////////

int isActiveConection; /// manage the connection of client

////////////////////////////USER INPUT///////////////////////////////////

//// get IP and Port of client to generate connection into server
void inputServerData(char** hisPeerIP, int* hisPeerPort){
    *hisPeerIP = new char[100];
    printf("A cual server me conecto?\n");
    printf("IP: "); 
    scanf("%s", *hisPeerIP); 
    printf("Puerto: ");
    scanf("%d", hisPeerPort);
    cin.ignore(1, '\n');
}

///////////////////////////PARSER CMD/////////////////////////////////

////// THIS IS A MANAGER TO FORMAT MSG WILL TO SENT INTO SERVER
//// process msg to format will analyze in server
string parseCmd(string cmd){
    string pack;
    /////cout << cmd << endl;
    vector<string> token = tokenizer(cmd);
    if(token.empty())
        pack = "error parsing";
    else if(token[0] == "nickname") /// command nickname
        pack = join({"1", len(token[1],2) ,token[1]});
    else if(token[0] == "list") /// command list
        pack = join({"2"});       
    else if(token[0] == "msg") /// command msg
    {
        string msg = parserGetMsg(cmd);       
        pack = join({"3", len(token[1],2), token[1], len(msg,3), msg});
    }
    else if(token[0] == "exit") /// command exit
        pack = join({"8"});
    else if (token[0] == "login"){ /// command login
        string user, pass;
        user = token[1];
        pass = token[2];
        pack = joinBySanti({"l", len(user, 2), len(pass, 2), user, pass});
        /////cout << pack << endl;
    }
    else if(token[0] == "all"){ /// command list in santi protocol
        pack = joinBySanti({"i"});
    }
    else if(token[0] == "talk"){
        string usr, msg_;
        usr = token[1];
        msg_ = cmd.substr(usr.size() + 6, cmd.size() - usr.size() - 6);
        pack = joinBySanti({"m", len(msg_, 3), len(usr, 2), msg_, usr});
    }
    else
        pack = "error parsing"; /// wrong found command
    return pack;
}

////////////////////////////////PACKS & REQUEST/RESPONSE/////////////////////////////////

/// generate responde from msg of server
string request2response(string pack){
    string response;
    switch(pack[0])
    {
        case '7': // msg
        {
	    response = join({"5"}); /// ok
            break;
        }
        default:
            response = errorMsg("Paquete no valido de server"); // response
    }
    return response;
}

////// THIS IS A MANAGER TO INTERPRET MSG FROM SERVER
//// generate response from pack of server
string response2string(string pack)
{
    string str;
    int idx = 2;

    if(pack[0] == '4'){
        string errmsg = parserGetField(pack, idx, 3); /// get msg of error
        str = errmsg; 
    }else if(pack[0] == '5'){
        str = "OK"; 
    }else if(pack[0] == '6'){
        string listmsg = parserGetField(pack, idx, 3); /// get list of nickname of current connected clients in server
        str = listmsg; 
    }else if(pack[0] == 'E'){
        int idxInternal = 1;
        str = parserGetFieldByString(pack, idxInternal, 20); /// error santi protocol
    }else if(pack[0] == 'I' && pack[1] == 'E'){
        int idxInternal = 2;
        str = parserGetFieldByString(pack, idxInternal, 20); /// error santi protocol
    }else {
        cout << "entramso en error!"<< pack << endl;
        sleep(100000);
        str = errorMsg("Comando no reconocido del server");         
    }
    return str;
}


///////////////////////////////////SEND/RECEIVE PACKS/////////////////////////////////
//// SEND

//// send msg to server
void sendPackagesToServer(int sockFD){
    do{
        string cmd, pack;
        printf(">>");
        getline(cin, cmd);
        pack = parseCmd(cmd); /// process msg
        cout << pack << endl;
	    if(pack == "8") // quit
            isActiveConection = false; /// close connection of client
        if(pack == "error parsing") /// if pack is error
            printf("Comando no valido\n");
	    else{
            // printf("Debug: envio [%s]\n", pack.c_str()); 
            ___write(sockFD, pack.c_str(), pack.size()); /// send message
	    }
    }while(isActiveConection);
}

//// RECEIVE

/// check if pack is a command
bool isRequest(string pack){
    return pack[0] == '7';// msg
}

/// check if pack is a command in santi protocol
bool isValidRequestSanti(string pack){
        return  pack[0] == 'M' || pack[0] == 'L' || (pack[0] == 'I' && pack[1] != 'E');// msg
} 

//// check if msg is part of santi protocol
bool isSantiProtocol(string pack){
    return pack[0] == 'E' || pack[0] == 'L'; // msg
}

//// clean terminal to display a correct way the msg
void printfClean(string reqres) // Evita que la salida consola descuadre con el >> del sendPackages()
{
     printf("\b\b%s\n>>", reqres.c_str()); // Elimino los '>>', imprimo, agrego otros '>>' 
     fflush(stdout);
}

//// print messge from server in client terminal
void printIsRequestIsMessage(string pack){
    if(pack[0] != '7') return;
    char buf[280]; //// create a temp buffer
    bzero(buf, 280); //// clean temp buffer
    int idx = 2;

    string nickFrom = parserGetField(pack, idx, 2); /// get nickname of client sent msg
    string msg = parserGetField(pack, idx, 3); /// get msg
    sprintf(buf, "%s: %s", nickFrom.c_str(), msg.c_str()); /// generate msg, store in buffer
    printfClean(string(buf)); /// clean terminal and display msg
}

//// print messge from server in client terminal by santy
void printIsRequestIsMessageBySanti(string pack){
    char buf[280]; //// create a temp buffer
    bzero(buf, 280); //// clean temp buffer
    int idx = 2;
    string msgClient = "";
    vector<int> sizeName;

    cout << "sdasd" << pack << endl;
    switch (pack[0]){
        case 'L':
            sprintf(buf, "%s: %s", "server", "ok! login valido"); /// generate msg, store in buffer
            printfClean(string(buf)); /// clean terminal and display msg
            break;
        case 'I': {
            idx = 1;
            int sizeN1 = parserGetFieldByInt(pack, idx, 2);
            for(int it = 0; it < sizeN1; it++){
                sizeName.push_back(parserGetFieldByInt(pack, idx, 2));
            }
            for(int it = 0; it < sizeName.size(); it++){
                msgClient += parserGetFieldByString(pack, idx, sizeName[it]) + ", ";
            }
            msgClient.pop_back();
            msgClient.pop_back();
            sprintf(buf, "%s: %s", "server", msgClient.c_str());
            printfClean(string(buf)); /// clean terminal and display msg
            break;
        }
        case 'M': {
            idx = 1;
            int sizeMsg = parserGetFieldByInt(pack, idx, 3);
            int sizeUser = parserGetFieldByInt(pack, idx, 2);
            string valueMsg = parserGetFieldByString(pack, idx, sizeMsg);
            string valueUser = parserGetFieldByString(pack, idx, sizeUser);

            sprintf(buf, "%s: %s", valueUser.c_str(), valueMsg.c_str());
            printfClean(string(buf)); /// clean terminal and display msg
            break;
        }
        default:
            break;
    }
    return ;
}

//////////////////////////////
//////////////////////////////
//////////////////////////////
/**
 * Purpose:
 *  receive msg of responde from server by client.
 * Params:
 *  - sockFD: id of connection
 * Return
 */ 
void receivePackagesFromServer(int sockFD){
    do{ 
        string pack(1000, '\0');
        ___read(sockFD, (char*)pack.c_str(), pack.size());
        // printf("Debug: recibo [%s]\n", pack.c_str());

        ///cout  << pack << endl;
        ///cout  << isRequest(pack) << endl;
        ///cout  << isValidRequestSanti(pack) << endl;
        string request, response; // Los packetes del server o son de tipo request, o response
        if(isRequest(pack)) /// check is pack is command
        {
            printIsRequestIsMessage(pack); /// print msg in terminal
            pack = request2response(pack); //// generate response of msg
            ___write(sockFD, pack.c_str(), pack.size());  // envio al server su respuesta
        }else if(isValidRequestSanti(pack)){
            printIsRequestIsMessageBySanti(pack);
        }
        else{
            response = response2string(pack); /// generate message to responde of pack
	        printfClean(response); /// clean terminal and display msg
        }
    }while(isActiveConection);
}

/// main core of client.
void client_Thread(){ 
    try
    {
        struct sockaddr_in stSockAddr;
        int sockFD, hisPeerPort;
        char* hisPeerIP; 
         // Crear socket
        sockFD = ___socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        
        // Conectar a peer server
        /// ingresar datos usuario
        inputServerData(&hisPeerIP, &hisPeerPort);
        /// setear variables(IP, PORT)
        memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(hisPeerPort);
        ___inet_pton(AF_INET, hisPeerIP, &stSockAddr.sin_addr);
    
        /// establecer conexion a server
        ___connect(sockFD, (struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
        printf("OK, conectado a server\n");
    
        // Enviar y Recibir paquetes
        thread t1(sendPackagesToServer, sockFD); /// generate thread to write of client by terminal
        thread t2(receivePackagesFromServer, sockFD); /// generate thread to read of client by terminal
        t1.join(); /// join thread from main thread of executor
        t2.join(); /// join thread from main thread of executor
        
        // Cerrar conexion 
        shutdown(sockFD, SHUT_RDWR);
        close(sockFD);
    }
    catch(string msg_error)
    {
        perror(msg_error.c_str());
        exit(EXIT_FAILURE);
    }
}

//////////////////////////////MAIN////////////////////////////////
// hostname -I
// Esta basado en modelo request-response
int main(int argc, const char** argv){
    while(true) // Al cerrar conexion puedo volver a abrir otra
    {
        isActiveConection = true;
        client_Thread();
    }
    return 0;
}




//////////////////////////////AUX/////////////////////////////
// Agrega una capa de seguridad a cada funcion
// No es necesario leerse todo esto, solo sirve para try-catch

//// create a socket
int ___socket(int domain, int type, int protocol){
    int sockFD = socket(domain, type, protocol);
    if (-1 == sockFD)
      throw string("cannot create socket");
    return sockFD;
}

int ___inet_pton(int af, const char* src, void* dst){
    int res = inet_pton(af, src, dst);
    if (0 > res)
      throw string("error: first parameter is not a valid address family");
    else if (0 == res)
      throw string("char string (second parameter does not contain valid ipaddress");
    return res;
}

//// generate a id of connection
void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen)
{
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


