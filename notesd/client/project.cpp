// notes specific headers
#include "/u/cs-212/common/global.h"
#include "container.h"
#include "project.h"
#include "extNTPfunctions.h"
#include "newcurses.h"

// our generic headers
#include "/u/cs-212/common/lists.h"
#include "/u/cs-212/common/str.h"

// standard headers
#include <curses.h>
#include <stdio.h>
#include <conio.h>

//A lot of this will be the exact same as the related functions for containers.
//However minor changes require that I duplicate it here

// names for color pairs. This is how curses works. Foreground_Background
enum {White_Black, Red_Black, Blue_Green, Cyan_Black, Red_Yellow);

Project::Project(oid_t ID){
  Object_List = new List<Contained_Object>;//open a list to hold info on stuff in this container
  My_ID = ID;
  Get_NTPObject(ID);//get ready to get to info

  String Buffer;

  int n=0;
  char *C;
  String Temp1;
  String Temp2;

  Read_NTPData(Buffer, 1);//get one byte; this will be an int which is the number of objects;

  int Objects = 0;
  sscanf(Buffer, %d, &Objects);
  int Objects_Received = 0;
  while(Objects_Received != Objects){
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
  
  n = 0;
  int Project_Members;
  int Members_Received = 0;
  Read_NTPData(Buffer,1);
  sscanf(Buffer, %d, &Project_Members);
  while(Members_Received != Project_Members)
    {
      Get_NTPchar(Buffer, 1024);
      
      if(!n%2)
	{
	  Temp1 = Buffer;
	  n++;
	}
      else
	{
	  Temp2 = Buffer;
	  n++;
	  Set_User(Temp1, Temp2);
	  Temp1.Erase();
	  Temp2.Erase();
	}
    }
}

oid_t Project::Interact()
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
  mvaddstr(1,1,this.Name);// print the mane of the current container in the upper left corner.
  mvaddstr(1,20,this.My_ID);//print the ID of the current container

  // print the names of the contained objects
  int row = 3;
  while(!Object_List.End()){
    move(row,3);
    (Object_List.Get_Current).Print_Object_Name();
    Object_List.Next();
    row++;
  }


  //Ok we've got the names printed and the first one highlighted.
  //Next I have to get ready to let the user select a Notes_Object
  //Find out how to set it to "DELAY" mode

  Object_List.Start_Iteration();//goto the beginning of the list
  High_Light(3,3,(Object_List.Get_Current).Name);
  row = 3;
  refresh();

  cbreak(CBREAK);//use the character imediatly, don't wait for a \n
  char Opt;
  while(Opt != /*I need to put the equivalent of the escape key here*/){
    move(3+Object.Where(),3);
    (Object_List.Get_Current).Print_Object_Name);//reprint old item with out highlight

  if(isalpha(Opt)) tolower(Opt);
  switch(Opt){
  case KEY_UP://if the key is the up arrow go to the previous object
    Object_List.Previous();
    High_Light(3+(Object_List.Where()),3,(Object_List.Get_Current).Name);//Highlight the new object
    refresh();
    break;
  case KEY_DOWN://if the key is the down arrow go to the next object
    Object_List.Next();
    High_Light(3+(Object_List.Where()),3,(Object_List.Get_Currnet).Name);
    refresh();
    break;
  case 'g':
    int n = 0;
    string TempStr;
    WINDOW *TempWin = newwin(2, 4, 11,37);//make a temporary window to show the number being entered
    while((char C = getch()) != '\n'){//or should this be '\r' ?
      if(isdigit){
        n = 10*n+(C-'0');//if it's a number append it to the existing one
        sprintf(TempStr,"%d", n);//put the number in a string so it can be printed
        mvwaddstr(TempWin, 1, 1, TempStr);//print it
        refresh();
      }
      else beep();
    }
    if(n > Object_list.Number_of_Items()) break;
    else{
      Object_List.Goto(n-1);//goto the appropriate object
      High_Light(3+(Object_List.Where()),3, (Object_List.Get_Current).Name);//Highlight the new object
    delwin(TempWin);
    }
    break;
  case: KEY_ENTER
    WINDOW *TempWin = newwin(2,20,11,30);
    String Temp;
    mvwaddstr(TwmpWin, 1,1 ,"Enter your user name"
    while((char C = getch()) != '\n')
    {
      if(isalph(C)) Temp.Append();
      else beep();
    }

    int Project_Member;
    if(Temp == Project_Head.User_Name)
    {
      Project_Member = 1;
    }
    else
    {
      Workers.Start_Iteration();
      while(!Workers.End() && !Project_Member){
        if(Temp == (Workers.Get_Current()).User_Name) Project_Member = 1;
        else Workers.Next();
      }
      if(!Project_Member){
        werase(TempWin);
        mvwaddstr(TempWin,1,2,"Invalid User Name");
      }
      else
      {
        oid_t Temp = (Object_List.Get_Current).My_ID;//get the ID of the selected object
        delete Object_List;
        return Temp;//return the ID of the selected object
      }
    break;

  /* All this stuff will be added later; this requires writing to disk
  case: 'a'//add another user
    WINDOW *TempWin = newwin(2,20,11,30);//make a temporary window
    String Temp;
    mvwaddstr(TwmpWin, 1,1 ,"Enter your user name"
    while((char C = getch()) != '\n')
    {
      if(isalph(C)) Temp.Append();
      else beep();
    }
    if(Temp == Project_Head.User_Name)
    {

    }
    else
    {
      werase(TempWin);
      mvwaddstr(TempWin,1,2,"Invalid User Name");
    }*/

endwin();
}

void Project::Print_Object_Name()
{
  attrset(COLOR_PAIR(Red_Yellow));
  addstr(Name);
}

void Project::Set_Object(String S1, String S2)
{
  Contained_Object Temp;// make a temporary struct to hold the info

  Temp.Object_Name = S1;// put in the name
  Temp.Object_ID = S2;// and the ID

  Append(Temp);
}

void Project::Set_User(String S1, String S2)
{
  Person Temp;
  Temp.User_Name = S1;
  Temp.Password = S2;

  Append(Temp);
}
