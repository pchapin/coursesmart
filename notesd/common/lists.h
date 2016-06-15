
#ifndef LIST_HPP
#define LIST_HPP

template<class T> class List
{
 private:
  struct List_Node{
    T Data;
    List_Node *Next;
    List_Node *Previous;
  };
  List_Node *First;
  List_Node *Last;
  List_Node *Current_Point;
 public:
  List();// main constructor
  List(const List<T> &Source);// constructor that sets new list equal to an old one
  ~List();// destructor
  void Prepend(const T &Source);//add an object to the beginning of the list
  void Append(const T &Source);//add an object to the end of the list
  void Insert(const T &Source);// add an object imediatly before the Current_Point
  void Delete();//remove the object immediatly before the Current_Point
  void Erase();//remove all objects from the list, but do not delete the list
  void Start_Iteration();//goto the beginning of the list
  void Goto_End();//goto the end of the list
  void Goto(int);//goto a specific place in the list
  int Beginning(); //returns 1 if at the beginning of the list 0 otherwise
  int End();//returns 1 if at the end of the list, 0 otherwise
  int Where();//returns the item number of Current_Point
  int Number_of_Items();//returns the number of items in the list
  void operator=(List<T> &);//set one list equal to another
  void Next();//goto the next object
  void Previous();// goto  the previous object
  T &Get_Current();//get the data of the current object
  T &Get_Next();// get the data of the next object
  T &Get_Previous();//get the data of the previous object
  T &Get_Data(int);//get the data of a specific object
};

#endif
