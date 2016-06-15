/***********************************************************************
  FILE           :message.h
  LAST REVISION  :March 22, 1996
  SUBJECT        :definition of the message class
  PROGRAMMER     :Rory Loughran

  Send questions/comments to:  rloughra@twilight.vtc.vsc.edu

************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "../common/global.h"
#include "note.h"

class Message:public Note_Object{

 private:
   List<String> text;    // Each String is one line. The list of lines
                            // makes up the message.
 public:
   Message(oid_t);       //constructor
   virtual oid_t Interact();
}; 

#endif

