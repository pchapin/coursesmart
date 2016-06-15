/****************************************************************************
FILE          : client.c
LAST REVISION : April 1996
SUBJECT       : Main program of Unix notes client.
PROGRAMMER    : Peter Chapin

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "../common/global.h"
#include "../common/ntp.h"
#include "../common/stack.h"
#include "container.h"
#include "message.h"
#include "note.h"
#include "project.h"

Stack<Note_Object *> Object_Stack;
  // Stack of objects have visited so far.


//
// Get_Object()
//
// This function constructs an object of the appropriate type by drawing
//   it off the server. It returns a pointer to the object or NULL if it
//   couldn't get the object for some reason.
Note_Object *Get_Object(oid_t Get)
{
  Note_Object *Result = 0;
  char Buffer[4+1];

  // Can we talk to the server?
  if (!Open_NTPConnection("twilight.vtc.vsc.edu")) {
    printf("client: Can't open NTP connection to twilight.vtc.vsc.edu\n");
    return Result;
  }

  if (!Get_NTPObject(Get)) {
    printf("client: Can't get object with id #%d.\n", (int)Get);
  }
  else {
    // Get the first four bytes of the object. This is assumed to be the
    //   object type ID (three digits) followed by a '\n' character.
    int Count = Read_NTPData(Buffer, 4);
    Buffer[4] = '\0';

    // This is the only place in the client program with a switch statement
    //   like this exists. Everywhere uses virtual functions to interact
    //   with a "generic" object.
    switch (atoi(Buffer)) {
      case 1: Result = new Container(Get); break;
      case 2: Result = new Message(Get);   break;
      case 3: Result = new Project(Get);   break;
      default:
	printf("client: Unknown object type ID in object #%d.\n", (int)Get);
	break;
    }
  }

  // We are done talking to the server.
  Close_NTPConnection();
  return Result;
}


// +++++++++++++++++++++++++++++++++++++
//           The Main Program
// +++++++++++++++++++++++++++++++++++++
int main()
{
  // Start the ball rolling by getting object #1 from the server.
  Note_Object *p = Get_Object(1);

  // Make sure we got it. If so, put it on our stack to start.
  if (p == 0) {
    printf("client: Can't get initial object from the server!\n");
    return 1;
  }
  Object_Stack.Push(p);

  oid_t Get_Me;

  // Do this stuff while the stack is not empty.
  do {

    // Interact with the current object. It will tell use what object
    //   we should get next.
    Get_Me = p->Interact();

    if (Get_Me == 0) {

      // The user wants to back up. Blow away the current top of stack.
      p = Object_Stack.Pop();
      delete p;

      // Get the new top level. This might be NULL if there is nothing
      //   to get. That is fine. The loop will break below.
      p = *Object_Stack.Get_Top();
    }

    else {

      // The user wants to go forward. Get the desired object and put it
      //   on the stack.
      p = Get_Object(Get_Me);
      Object_Stack.Push(p);
    }

  } while (p != 0);

  return 0;
}

