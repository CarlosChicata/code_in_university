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
 *  comparing 2 string are same.
 * Params
 *  - a (string) : message first.
 *  - b (string) : message second.
 * return boolean is same or not.
 */
bool comparingString(string a, string b){
  int minSize = min(a.length(), b.length());
  bool rpta = true;

  for( int idx = 0; idx < minSize; idx++){
    if(a[idx] != b[idx]){
      rpta = false;
      break;
    }
  }

  return rpta;
}


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
