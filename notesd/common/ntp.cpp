#include <string.h>
#include <stdio.h>
#include "ntp.h"
#include "connect.h"
// #include whatever else

String First301;

int Open_NTPConnection(char *Host_Name) 
{
  if(Open_Connection(Host_Name)==0) {
    fprintf(stderr,"notes: can't open NTP connection to %s\n",Host_Name);
    return 0;
  }
  String Banner_String;
  if(Receive_Line(Banner_String)==0) {
    return 0;    
  }
  if(strstr(Banner_String,"200")) return 1;
  else return 0;
}

inline void Close_NTPConnection() { Close_Connection(); }
  
int Send_200() 
{
  return Send_Line("200 How's it going?");
}

int Send_201()
{
  return Send_Line("201 What is this for?");
}

int Send_301(char *Buffer)
{
  return Send_Line(Buffer);
}

int Send_302()
{
  return Send_Line("302 This is the palce to be");
}

int Send_401()
{
  return Send_Line("401 what did you do to get here?");
}

int Send_501()
{
  return Send_Line("501 This is the last one");
}

int Get_Line(String &Client_Command)
{
  char buffer[128+1];
  int Result=Receive_Line(buffer);
  Client_Command=buffer;
  return Result;
}
  
int Read_NTPData(char *Buffer, int Size)
{
  if(First301.Length()) {      // Do we have a message waiting?
    strcpy(Buffer,First301);   // If so, put it in the buffer and bypass the next receive
    First301.Erase();          // then clear the message out
  }
  else if(Receive_Line(Buffer)==0) {
    fprintf(stderr,"client: Receive_Line failed");
    return 0;            //indicate failure
  }
  if((strncmp(Buffer,"301 ",4))==0) {
    Buffer+=4;
    return strlen(Buffer);
  }
  if((strncmp(Buffer,"302 ",4))==0) return 0;  // 302 means we're finished?
  // What to do here?
  return 0;
}

int Get_NTPObject(oid_t Object_ID)
{
  char Command[20];
  sprintf(Command,"GET %ld",Object_ID);
  Send_Line(Command);
  return Receive_Line(First301);
}



