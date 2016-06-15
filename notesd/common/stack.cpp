//-------------------------------------------------------
//FILE          : stack.C
//LAST REVISION : March 1996
//SUBJECT       : The Stack Template
//PROGRAMMER    : Adam Trojanowski
//
// it even works, too!
//-------------------------------------------------------


#include "stack.h"
#include "lists.h"


template<class T> void Stack<T>::Push(const T &Thing)
{
  A_Stack.Prepend(Thing);
}


template<class T> T Stack<T>::Pop()
{
  T Thing;
  
  if (A_Stack.Number_of_Items()==0) return 0;
  A_Stack.Start_Iteration();
  Thing=A_Stack.Get_Current();
  A_Stack.Delete();
  return Thing;
}

template<class T> T *Stack<T>::Get_Top()
{
  A_Stack.Start_Iteration();
  return &(A_Stack.Get_Current());
}


template<class T> int Stack<T>::Item_Count()
{
  return A_Stack.Number_of_Items();
}









