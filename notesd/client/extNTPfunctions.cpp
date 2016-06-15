#include "extNTPfunctions.h"
#include "../common/ntp.h"

#include "../common/str.h"

void Get_NTPchar(String Buffer, int Length)
{
  char Temp = new char[length+1];
  Temp[length] = '\n';
  char *p = Temp;
  
  Read_NTPData(Temp, Length);
  
  while(1){
    if(*p == '\0') break;
    else if(*p == '\n') Read_NTPData(Temp, length);
    else{
      Buffer.Append(*p);
      p++;
    }
  }
}


