#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <utility>

using namespace std;

//// METHODS TO MANAGE MESSAGE

/**
 * Purpose: 
 *  Convert char* to string
 * Params:
 *  - buffer (char *) : array of char.
 * Return a string.
 */
string buffertToString(char* buffer){
  string s(buffer);
  return s;
}

/**
 * Purpose:
 *  segmentation string by delimite
 * Params:
 *  - msg (string) : message will segment.
 *  - delimiter (string): token to create segmentation in string
 * Return a pairt: first is segmentation and other if rest of string.
 */ 
pair<string, string> segmentationString(string msg, string delimiter){
  size_t pos = 0;
  pair<string,string> rpta = make_pair("", msg);

  if((pos = msg.find(delimiter) ) != string::npos){
    rpta.first = msg.substr(0, pos);
    msg.erase(0, pos + delimiter.length());
    rpta.second = msg;
  }

  return rpta;
}

/**
 * Purpose:
 *  analyze the command and get data to process.
 * Params:
 *  - command (string) : message in txt.
 *  - connect (int) : ID of connection client.
 * Return a pair with message to send and ID connection will receive
 * Message.
 
pair<int, string>  manageCommands(string command, int connect){
  pair<int, string> rpta = make_pair(connect, "WRONG!");
  pair<string, string> processComm = segmentationString(command, " ");
  
  /// command options
  if(processComm.first == "name" ){
    //// store name in server
    conn_list[connect] = processComm.second;
    reverse_conn_list[processComm.second] = connect;
    rpta.second = "Save name!";
  }else if(processComm.first == "msg"){
    //// send msg to other client
    processComm = segmentationString(processComm.second, " ");
    rpta.first = reverse_conn_list[processComm.first];
    rpta.second = processComm.second;
  }else{
    /// wrong command
    rpta.second = "Invalid Commad!";
  }
  return rpta;
}
*/

////  METHODS OF SOCKET

/**
 * Purpose:
 *  read message from client.
 * Params:
 *  - connect: id of connection.
 *  - buffer: buffer to store message.
 *  - size_buffer: max size of buffer.
 * Return message or error
 */
void readSocket(int connect, char buffer[], int size_buffer){
    int n = read(connect, buffer, size_buffer);
    if(n < 0) 
      throw string("ERROR reading from socket");
}

/**
 * Purpose:
 *  write message to client.
 * Params:
 *  - connect: id of connection.
 *  - msg: message of client.
 *  - size_buffer: max size of buffer.
 * Return error or None.
 */ 
void writeSocket(int connect, const char* msg, int size_buffer)
{
    int n = write(connect, msg, size_buffer);
    if(n < 0)
      throw string("ERROR writing to socket");
}