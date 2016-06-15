/****************************************************************************
FILE          : note.h
LAST REVISION : March 1996
SUBJECT       : Definition of Note_Object abstract base class
PROGRAMMER    : Peter Chapin

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef NOTE_H
#define NOTE_H

#include "../common/global.h"
#include "../common/lists.h"
#include "../common/str.h"

class Note_Object {

protected:

  String      Name;         // Every object has a name.
  List<oid_t> Attachments;  // Every object potentially has attachments.
  oid_t       Parent;       // Every object (but the root) has a parent.
  oid_t       Previous;     // If this is an attachment, it has a previous.
  oid_t       My_ID;        // For possible reference.

public:

  Note_Object() : Name("NO-NAME"), Parent(0), Previous(0), My_ID(0) { }
    // The constructor insures that the members have sensible defaults.
    //   The constructors for the derived classes should update the
    //   members as appropriate.

  virtual ~Note_Object() { }
    // The destructor must be declared virtual to insure that deleting a
    //   pointer to Note_Object will actually cause the destructor of
    //   the derived object to execute. Note_Object's destructor otherwise
    //   has no duties.

  virtual oid_t Interact() = 0;
    // Interact with this object. Returns the object ID of the next
    //   object the user wants to manipulate. Returns 0 if the user
    //   wants to 'back up.' This is a pure virtual function because
    //   it makes no sense to interact with a plain Note_Object. You
    //   must actually interact with an object from one of the derived
    //   classes. As a result of being a pure virtual function, you
    //   cannot declare a Note_Object.

};

#endif

