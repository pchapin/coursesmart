/******************************************************************************
  FILE           :   server.C
  LAST REVISION  :   March 1996
  SUBJECT        :   Server
  PROGRAMMER     :   Elysian Graham

 ***************************************************************************/  

#include <stdio.h> 
#include "../common/connect.h"
#include "../common/ntp.h"
#include <string.h>
#include "../common/str.h"
#include "log.h"

void Service_Connection()
{
 String Command;
 int Count;
 char Buffer[128];
 
 Send_200();
 Get_Line(Command);                           //read string
                                             //check for get or quit
 if(strcmp(Command,"get") == 0)                 
   {         
     Send_401();

//     if(Send_301(Count, Buffer) == 0 )
//       Send_401();                      //object not found
//     else                           //get what client asks for  
//       {
//	 while(Buffer!=EOF)
//	   {
//	     sscanf(Buffer,);                   //scan the buffer for eof
//	   }
//	 Send_302();                       //DONE no more object data to send
//       }
   }
 else if(strcmp(Command, "quit") == 0)
   {
     Send_201();               //quit
   }
 else
   {
     Send_501();                            //Unknown System Error
   }
 
}

int main( )
{
  if (Open_Log() == 0) return 1;

// this programmer does not have a clue!

  if(Prepare_Socket() == 0)
    {
      Write_Log("Failed to open socket!");
    }   
  
  Main_Loop(Service_Connection);
		  
  Close_Log();
  return 0;
}





