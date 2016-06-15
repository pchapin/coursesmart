#ifndef CONTAINER_H
#define CONTAINER_H

#include "note.h"
#include "../common/lists.h"

struct Contained_Object // This holds the name and ID of a Child
{
  String Object_Name;
  oid_t Object_ID;
};

class Container : public Note_Object {
 protected:
  List<Contained_Object> Object_List; // These are the contained objects

  public:
    Container(oid_t ID);
  // This function will get the List of Contained_Objects
    oid_t Interact();
  // This function will return the ID of the currently selected Child
    void Print_Object_Name();
  // This function will print the name of this object when its parent Container is examined. Each Note_Object will have one of these functions so that the menu can be color coded.
    void Set_Object(String S1,String S2);
  // This function will take two strings (the first being the Name and the second being the ID of the new obejct
};

#endif

















