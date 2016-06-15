//-------------------------------------------------------
//FILE          : log.C
//LAST REVISION : April 1996
//SUBJECT       : message logging stuff
//PROGRAMMER    : Adam Trojanowski
//
// Looks finished to me!
//-------------------------------------------------------


#include "../common/config.h"
#include <stdio.h>
#include "../common/str.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>


FILE *Log_File;

int Open_Log()
{
  String *Log_Path;
  Register_Parameter("Log_File", "./log.file");
  if ((Log_Path=Lookup_Parameter("Log_File"))==0) return 0;
  Log_File=fopen(*Log_Path,"a");
  // Log_File=fopen("/u/atrojano/notesd/log.file","a");
  if (Log_File==0) return 0;
  else return 1;
}


void Close_Log()
{
  fclose(Log_File);
}


void Write_Log(char *Format, ...)
{
va_list Args;
char Buffer[1024+1];
long Time_Stamp;

va_start(Args,Format);
vsprintf(Buffer,Format,Args);

time(&Time_Stamp);
char *s;
s=ctime(&Time_Stamp);
char *p;
p=s;
while (*p!='\n')p++;
*p=']';
fprintf(Log_File,"[%s->%s\n",s,Buffer);
fflush(Log_File);
va_end(Args);
}













