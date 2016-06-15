#ifndef PROJECT_H
#define PROJECT_H

#include "note.h"
#include "../common/lists.h"

struct Person {
  String User_Name;
  String Pass_Word;
};

class Project:public Container{
 private:
  Person Project_Head;
  List<Person> Workers;

 public:
  Project(oid_t ID);
  ~Project();
  oid_t Interact();
  void Print_Object_Name();
};

#endif







