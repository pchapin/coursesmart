#include <curses.h>
#include <string.h>
#include "newcurses.h"
#include "../common/lists.h"

Red_Yellow = 4;

void High_Light(int row, int col, *char source, int length = strlen(source)){
  attron(COLOR_PAIR(Red_Yellow));
  mvaddstr(row, col, source);
}

void Messasge_Box(*char source){
  List<String> Words1;
  String Buffer;

  while(char getch(char C) != '\0'){
    if(C != ' '){
      Buffer.Append(C);
    }
    else{
      Words1.Append(Buffer);
      Buffer.Erase();
    }
  }

  
    
}









