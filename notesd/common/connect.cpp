/****************************************************************************
FILE          : connect.c
LAST REVISION : May 8 1996
SUBJECT       : Low level connection functions.
PROGRAMMER    : Joshua W. Flood

These functions provide basic TCP/IP connectivity for the notes
client and server.

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NO 0
#define YES 1

#define LINE_LENGTH 128

static int Socket_Descriptor;
static int Listen_Socket;


/*static int New_Socket;*/

/****************************************************************************
              Look Up the Service
 ****************************************************************************/

static struct servent *LookUpService(char *Name, char *Proto)
{
  struct servent *Service_Info;

  Service_Info = getservbyname(Name,Proto);
  if (Service_Info == 0)
    {
/*      Error(2,0); */
    }
  
  else
    {
/*      printf("Found the %s service.\n",Name); */
    }

  return Service_Info;
}



/****************************************************************************
              Look Up The Server
 ****************************************************************************/

static struct hostent *LookUpServer(char *Name)
{
  struct hostent *Host_Info;

  Host_Info = gethostbyname(Name);         /* Look up Host Name     */

  if (Host_Info == 0)                   /* Make sure Host exists */
    {
/*      Error(1,0);*/
    }

  else
    {
/*      printf("Found %s.\n",Name); */
    }
  return Host_Info;

}



/*---------------------*/
/* Used by the client. */
/*---------------------*/

int Open_Connection(char *Host_Name)
  /* Estabilishes a connection with a notes server host with the specified
       name. This function returns 0 if it fails and 1 otherwise. */

{
  struct servent *Service_Info;
  struct hostent *Host_Info;

  struct sockaddr_in Socket_Address;

                                           /* Look up Service */
  Service_Info = LookUpService("notes","tcp");

  Host_Info = LookUpServer(Host_Name);     /* Look up Server */


/****************************************************************************
             Establish Connection
 ****************************************************************************/

  bzero(&Socket_Address, sizeof(Socket_Address));
  bcopy(
	Host_Info->h_addr_list[0],        /* Source          */
	&Socket_Address.sin_addr,         /* Destination     */
	Host_Info->h_length               /* Number of Bytes */
  );

 
  Socket_Address.sin_family = Host_Info->h_addrtype;

  Socket_Address.sin_port = Service_Info->s_port;

  Socket_Descriptor = socket(AF_INET, SOCK_STREAM, 0);

  if (Socket_Descriptor == 0)
    {
      return 0;
    }

  if (connect
      (Socket_Descriptor, (const struct sockaddr *)&Socket_Address, sizeof(Socket_Address)) == -1)

    {
      return 0;
    }

  return 1;

}


void Close_Connection(void)
  /* Closes a previously established connection with a notes server. If
       no connection has been established this function has no effect. */
{

/*  close(Socket_Descriptor);*/
    close(Socket_Descriptor);
  return;
}


/*-----------------------------------------*/
/* Used by both the client and the server. */
/*-----------------------------------------*/

int Send_Line(char *Line)
  /* Sends a line of text to the remote machine. A '\n' character will
       be added to the end of the line by this function. Returns 0 if
       it fails and 1 otherwise. */
{

  if (Socket_Descriptor == 0)
    {
      fprintf(stderr,"ERROR: Write to Socket = %d.\n",Socket_Descriptor);
      return(0);
    }
   
  if ((strlen(Line)) > (LINE_LENGTH-1))
    {
      Line[LINE_LENGTH-1] = '\0';
      Line[LINE_LENGTH-2] = '\n';
    }

  else
    {      
      Line[strlen(Line)] = '\n';
    }

  write(Socket_Descriptor, Line, (strlen(Line)));

  return 1;

}

int Receive_Line(char *Line)
  /* Gets a line of text from the remote machine. The incoming '\n'
       character will be stripped off. Returns 0 if it fails and 1
       otherwise. */

{
  int Done;
  int Byte_Count;
  int length;

  char Line_Buffer[LINE_LENGTH];
  char rLine[32+1];
  char ch;

  Done = NO;
  Byte_Count = 0;
  length = 0;


  Line_Buffer[0] = '\0';
  rLine[0] = '\0';


  if (Socket_Descriptor == 0)
    {
      fprintf(stderr,"ERROR Read from Socket = %d.\n",Socket_Descriptor);
      return 0;
    }

  do
  
    {

      length = (read(Socket_Descriptor, rLine, 32));
      
      rLine[length] = '\0';

      strcat(Line_Buffer,rLine);
      rLine[0] = '\0';

      if ((Line_Buffer[strlen(Line_Buffer)-1]) == '\n') 
	
	{
	  Done = YES;
	  fprintf(stderr,"Done = Yes\n");
	}

      fprintf(stderr,"length: %d\n",length);
      Byte_Count += length;
      fprintf(stderr,"Byte_Count: %d\n",Byte_Count);


      length = 0;

    }
     
  while ((Byte_Count < (LINE_LENGTH)) && Done == NO);

  fprintf(stderr,"Loop Terminated with Byte_Count = %d\n",Byte_Count);


  /* Clear socket buffer  */

  if (Byte_Count > (LINE_LENGTH))
    {

      do
	{
	  read(Socket_Descriptor, &ch, 1);
	}
      while (ch != '\n');
    }

  Line_Buffer[(strlen(Line_Buffer))] = '\0';

  strcpy(Line,Line_Buffer);

  return 1;
}

/*---------------------*/
/* Used by the server. */
/*---------------------*/

int Prepare_Socket(void)
  /* This function is used by the notes server to prepare a socket for
       incoming connections. */

{

  struct servent *Service_Info;

  struct sockaddr_in Socket_Address;

                                           /* Look up Service */
  Service_Info = LookUpService("notes","tcp");

                                           /* Initialize Socket_Address */
  bzero(&Socket_Address, sizeof(Socket_Address));

                                           /* Copy port info */
  Socket_Address.sin_port = Service_Info->s_port;
  
                                           /* Open Socket */
  Listen_Socket = socket(AF_INET, SOCK_STREAM, 0);

  if(Listen_Socket == 0)
    {
      return 0;
    }

  if(
     bind(Listen_Socket, (const struct sockaddr *)&Socket_Address, sizeof(Socket_Address)) < 0)
    {
      return 0;
    }

  listen(Listen_Socket, 5);                  /* Listen to socket */

  return 1;
}   


void Main_Loop(void (*Service_Connection)(void))
  /* This function is used by the server to wait for incoming connections.
       For each connection that comes in, this function forks() and then
       calls the function pointed at by Service_Connection in the child
       process. */

{

  
  while (1)
    {
      struct sockaddr_in From;

      int Length = sizeof(struct sockaddr_in);

      int fork_level;

      Socket_Descriptor = accept(Listen_Socket, (struct sockaddr *)&From, &Length);

/*      fprintf(stderr,"SERVER: Accept = %d\n",Socket_Descriptor);
*/

      if (Socket_Descriptor < 0)
	{
	  fprintf(stderr,"SERVER ERROR: New Socket = %d",Socket_Descriptor);
	}
      
      if ((fork_level = fork()) == 0)
	{
          close(Listen_Socket);
	  Service_Connection();
	  close(Socket_Descriptor);
	  exit(0);
	}



      else if(fork_level == -1)
	{

	  fprintf(stderr,"SERVER ERROR: Unable to Fork.\n");
	 
	}

      close(Socket_Descriptor);
    }
}

















