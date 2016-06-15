
#include "lists.h"

#define TEMP template<class T>


TEMP List<T>::List()
{
  First = Last = Current_Point = 0;
}

TEMP List<T>::List(const List<T> &Source)
{
  List_Node *p = Source.First;
  First = 0;
  Last = 0;
  Current_Point = 0;

  while(p!=0){
    Append(p->Data);
    p=p->Next;
  }
}

TEMP List<T>::~List()
{
  Current_Point = First;
  if(First == 0){
    while(First != Last){
      Delete();
    }
    Delete();
  }
}

TEMP void List<T>::Prepend(const T &Source)
{
  if(First == 0)
  {
    List_Node *New_Node = new List_Node;
    First = New_Node;
    New_Node->Next = New_Node;
    New_Node->Previous = New_Node;
    Last = New_Node;
    Current_Point = New_Node;
    New_Node->Data = Source;
    Current_Point = New_Node;
  }
  else
  {
    List_Node *New_Node = new List_Node;
    New_Node->Next = First;
    New_Node->Previous = Last;
    First = New_Node;
    New_Node->Data = Source;
    New_Node->Previous->Next = New_Node;
    New_Node->Next->Previous = New_Node;
  }
}

TEMP void List<T>::Append(const T &Source)
{
  if(First == 0)
  {
    List_Node *New_Node = new List_Node;
    First = New_Node;
    New_Node->Next = New_Node;
    New_Node->Previous = New_Node;
    Last = New_Node;
    Current_Point = New_Node;
    New_Node->Data = Source;
    Current_Point = New_Node;
  }
  else
  {
    List_Node *New_Node = new List_Node;
    New_Node->Next = First;
    New_Node->Previous = Last;
    Last = New_Node;
    New_Node->Data = Source;
    New_Node->Previous->Next = New_Node;
    New_Node->Next->Previous = New_Node;
  }
}

TEMP void List<T>::Insert(const T &Source)
{
  if(First == 0)
  {
    List_Node *New_Node = new List_Node;
    First = New_Node;
    New_Node->Next = New_Node;
    New_Node->Previous = New_Node;
    Last = New_Node;
    Current_Point = New_Node;
    New_Node->Data = Source;
  }
  else
  {
    List_Node *New_Node = new List_Node;
    Current_Point->Previous->Next = New_Node;
    New_Node->Next = Current_Point;
    New_Node->Previous = Current_Point->Previous;
    Current_Point->Previous = New_Node;
    New_Node->Data = Source;
  }
}

TEMP void List<T>::Delete()
{
  List_Node *p = Current_Point;
  if(First != 0){
    if(First == Last)
      {
	First = Last = Current_Point =0;
	delete p;
      }
    else
      {
	Current_Point->Previous->Next = Current_Point->Next;
	Current_Point->Next->Previous = Current_Point->Previous;
	if(Current_Point == Last) Last = Current_Point = Current_Point->Previous;
	else if(Current_Point == First) First = Current_Point = Current_Point->Next;
	else Current_Point = Current_Point->Next;
	delete p;
      }
  }
}

TEMP void List<T>::Erase()
{
  Current_Point = First;
  if(First != 0){
    while(First != Last){
      Delete();
    }
    Delete();
  }
}

TEMP void List<T>::Start_Iteration()
{
  Current_Point = First;
}

TEMP void List<T>::Goto_End()
{
  Current_Point = Last;
}

TEMP void List<T>::Goto(int Place)
{
  Start_Iteration();
  for(int n=0;n<Place;n++){
    Next();
  }
}

TEMP void List<T>::Next()
{
  Current_Point = Current_Point->Next;
}

TEMP void List<T>::Previous()
{
  Current_Point = Current_Point->Previous;
}

TEMP void List<T>::operator=(List<T> &Source)
{ 
  Erase();
  Source.Current_Point = Source.First;
  while(&(Source.Current_Point) != &(Source.Last)){
    List_Node *New_Node = new List_Node;
    *New_Node = *(Source.Current_Point);
    Source.Current_Point = Source.Current_Point->Next;
  }
}   

TEMP int List<T>::Beginning()
{
  if(Current_Point == First) return 1;
  else return 0;
}

TEMP int List<T>::End()
{
  if(Current_Point == Last) return 1;
  else return 0;
}

TEMP int List<T>::Number_of_Items()
{
  if(First == 0) return 0;
  
  List_Node *p = First;
  int n = 1;
  while(p != Last)
    {
      p = p->Next;
      n++;
    }
  return n;
}

TEMP int List<T>::Where()
{
  List_Node *p = First;
  int n = 0;
  while(p != Current_Point)
    {
      n++;
      Next();
    }
  return n;
}

TEMP T& List<T>::Get_Current()
{
  return Current_Point->Data;
}

TEMP T& List<T>::Get_Next()
{
  return Current_Point->Next->Data;
}

TEMP T& List<T>::Get_Previous()
{
  return Current_Point->Previous->Data;
}

TEMP T& List<T>::Get_Data(int n)
{
  List_Node *p = First;

  for(int m=0;m<n;m++)
  {
    p = p->Next;
  }
  return p->Data;
}







