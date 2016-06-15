/****************************************************************************
FILE          : ntp.h
LAST REVISION : March 1996
SUBJECT       : Functions to implement the notes transfer protocol.
PROGRAMMER    : 

These functions build on those declared in connect.h to do NTP.

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef NTP_H
#define NTP_H

#include "global.h"
#include "str.h"

//--------------
// Client side. 
//--------------

int Open_NTPConnection(char *Host_Name);
  // This opens a TCP/IP connection to the indicated notes server and
  //   verifies that the server sends a legitimate banner. Returns 0
  //   if failure, 1 otherwise.

void Close_NTPConnection();
  // This closes a previously opened connection. If no connection was
  //   open this function has no effect.

int Get_NTPObject(oid_t Object_ID);
  // This begins the process of getting a note object from the server.
  //   It issues the "GET" command and waits for some kind of
  //   acknowledgement. Returns 0 if failure, 1 otherwise.

int Read_NTPData(char *Buffer, int Size);
  // Reads Size bytes of raw note object data and loads it into the space
  //   pointed at by Buffer. This function returns the actual number of
  //   bytes read. It returns zero if there is nothing more to read. Call
  //   this function repeatedly to extract the body of a note object.

//-------------
// Server side.
//-------------

int Send_200();
int Send_201();
int Send_301(int Count, char *Buffer);
int Send_302();
int Send_401();
int Send_501();
  // The functions above generate the responses implied by their names.
  //   They all return 0 if they fail, 1 otherwise.

int Get_Line(String &Client_Command);
  // This function extracts a line of text from the client. It returns
  //   0 if it fails and 1 otherwise.

#endif

