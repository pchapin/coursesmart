/****************************************************************************
FILE          : stack.h
LAST REVISION : March 1996
SUBJECT       : A generic stack template.
PROGRAMMER    : Adam Trojanowski

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef STACK_H
#define STACK_H
#include "lists.h"

template<class T> class Stack {

private:
  List<T> A_Stack;

public:

  void Push(const T &);
    // Pushes the parameter onto the stack.

  T Pop();
    // Pops the stack and returns the old top item. Popping a stack of
    //   size zero is undefined.

  T *Get_Top();
    // Returns a pointer to the top item on the stack. Returns NULL if
    //   the stack is empty.

  int Item_Count();
    // Returns the number of items on the stack.
};

#endif

