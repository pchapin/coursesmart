// notes specific headers
#include "../common/ntp.h"
#include "../common/global.h"
#include "container.h"
#include "extNTPfunctions.h"
#include "newcurses.h"

// our generic headers
#include "../common/lists.h"
#include "../common/str.h"

// standard headers
#include <curses.h>
#include <stdio.h>
#include <ctype.h>

// names for color pairs. This is how curses works. Foreground_Background
enum {White_Black, Red_Black, Blue_Green, Cyan_Black, Red_Yellow};

Container::Container(oid_t ID){
  List<Contained_Object> *Object_List = new List<Contained_Object>;
  //open a list to hold info on stuff in this container

  My_ID = ID;
  Get_NTPObject(ID);//get ready to get to info

  String Buffer;

  int n=0;
  String Temp1;
  String Temp2;

  int Objects;
  int Objects_Received = 0;
  Read_NTPData(Buffer, 1);// get one byte which will hold the number of objects
  sscanf(Buffer, "%d", &Objects);
  while(Objects != Objects_Received){
    Get_NTPchar(Buffer,1024); //This function will get a character from the buffer Buffer, when it gets to the end it will try and get more data
    
    if(!n%2){
      Temp1 = Buffer;
      n++;
    }
    else{
      Temp2 = Buffer;
      n++;
      Set_Object(Temp1, Temp2);
      Temp1.Erase();
      Temp2.Erase();
      Objects_Received++;
    }
  }
}
	  

oid_t Container::Interact()
{
  initscr();
  start_color();

  // set all of the color pairs
  init_pair(0,7,0);
  init_pair(1,1,0);
  init_pair(2,4,2);
  init_pair(3,6,0);
  init_pair(4,1,3);

  attron(COLOR_PAIR(Red_Black));// set the color for the border
  
  box(stdscr, 0,0);// this will put a border around the screen
  mvaddstr(1,1,Name);// print the mane of the current container in the upper left corner.
  String ID_Temp;
  sprintf(ID_Temp, "%d", My_ID);
  mvaddstr(1,20,ID_Temp);//print the ID of the current container

  // print the names of the contained objects
  int row = 3;
  while(!Object_List.End()){
    move(row,3);
    (Object_List.Get_Current()).Print_Object_Name();
    Object_List.Next();
    row++;
  }

  
  //Ok we've got the names printed and the first one highlighted. 
  //Next I have to get ready to let the user select a Notes_Object
  //Find out how to set it to "DELAY" mode

  Object_List.Start_Iteration();//goto the beginning of the list
  High_Light(3,3,(Object_List.Get_Current()).Object_Name, ((Object_List.Get_Current()).Object_Name).Length());
  row = 3;
  refresh();

  cbreak();//use the character imediatly, don't wait for a \n
  char Opt = '/0';
  while(Opt != '\n' /*I need to put the equivalent of the escape key here*/){
    move(3+Object_List.Where(),3);
    ((Object_List.Get_Current()).Print_Object_Name());//reprint old item with out highlight
  if(isalpha(Opt)) tolower(Opt);
  switch(Opt){
  case KEY_UP://if the key is the up arrow go to the previous object
    Object_List.Previous();
    High_Light(3+(Object_List.Where()),3,(Object_List.Get_Current()).Object_Name,((Object_List.Get_Current()).Object_Name).Length());//Highlight the new object
    refresh();
    break;
  case KEY_DOWN://if the key is the down arrow go to the next object
    Object_List.Next();
    High_Light(3+(Object_List.Where()),3,(Object_List.Get_Current()).Object_Name,((Object_List.Get_Current()).Object_Name).Length());
    refresh();
    break;
  case 'g':
    int n = 0;
    String TempStr;
    WINDOW *TempWin = newwin(2, 4, 11,37);//make a temporary window to show the number being entered
    char C;
    while((C = getch()) != '\n'){//or should this be '\r' ?
      if(isdigit){
	n = 10*n+(C-'0');//if it's a number append it to the existing one
	sprintf(TempStr,"%d", n);//put the number in a string so it can be printed
	mvwaddstr(TempWin, 1, 1, TempStr);//print it
	refresh();
      }
      else beep();
    }
    if (n > Object_List.Number_of_Items()) break;
    else{
      Object_List.Goto(n-1);//goto the appropriate object
      High_Light(3+(Object_List.Where()),3, (Object_List.Get_Current()).Object_Name,((Object_List.Get_Current()).Object_Name).Length());//Highlight the new object
    }
    break;
  case KEY_ENTER:
    oid_t Temp = ((Object_List.Get_Current()).ID); //get the ID of the selected object
    endwin();
    return Temp;//return the ID of the selected object
   }
  }
}

void Container::Print_Object_Name()
{
  attrset(COLOR_PAIR(Blue_Green));
  addstr(Name);
}

void Container::Set_Object(String S1, String S2)
{
  Contained_Object Temp;// make a temporary struct to hold the info
  
  Temp.Object_Name = S1;// put in the name
  Temp.Object_ID = S2;// and the ID

  Object_List.Append(Temp);
}
      
Container::~Container()
{
  delete *Object_List;
}





















