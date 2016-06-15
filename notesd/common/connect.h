/****************************************************************************
FILE          : connect.h
LAST REVISION : April 1996
SUBJECT       : Low level connection functions.
PROGRAMMER    : 

These functions provide basic TCP/IP connectivity for the notes
client and server.

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef CONNECT_H
#define CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------*/
/* Used by the client. */
/*---------------------*/

int Open_Connection(char *Host_Name);
  /* Estabilishes a connection with a notes server host with the specified
       name. This function returns 0 if it fails and 1 otherwise. */

void Close_Connection(void);
  /* Closes a previously established connection with a notes server. If
       no connection has been established this function has no effect. */

/*-----------------------------------------*/
/* Used by both the client and the server. */
/*-----------------------------------------*/

int Send_Line(char *Line);
  /* Sends a line of text to the remote machine. A '\n' character will
       be added to the end of the line by this function. Returns 0 if
       it fails and 1 otherwise. */

int Receive_Line(char *Line);
  /* Gets a line of text from the remote machine. The incoming '\n'
       character will be stripped off. Returns 0 if it fails and 1
       otherwise. */

/*---------------------*/
/* Used by the server. */
/*---------------------*/

int Prepare_Socket(void);
  /* This function is used by the notes server to prepare a socket for
       incoming connections. */

void Main_Loop(void (*Service_Connection)(void));
  /* This function is used by the server to wait for incoming connections.
       For each connection that comes in, this function forks() and then
       calls the function pointed at by Service_Connection in the child
       process. */

#ifdef __cplusplus
}
#endif

#endif

