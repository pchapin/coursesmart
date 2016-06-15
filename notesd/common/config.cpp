/*****************************************************************
  FILE          : config.C
  LAST REVISION : March 1996
  SUBJECT       : Source code for configuration file manager.
  PROGRAMMER    : Damon DeCoste && Seth Seeger

Please send compliments and monetary donations to:
  Damon DeCoste
  teapot@teatime.vtc.vsc.edu

Please send bug reports, flames, death threats to:
  Seth Seeger
  sseeger@snacktime.vtc.vsc.edu

*******************************************************************/

#include "../common/config.h"
#include "../common/lists.h"
#include "../common/str.h"
#include <stdio.h>
#include <string.h>
#include "../common/entry.h"

static int GetLine(String &Buffer, FILE *File);
  //  Gets a complete line from the File.
  //  Returns the length of the line.

static List<Entry> Dictionary;

String *Lookup_Parameter (const char *Name)
{
  Entry TempHold;

  Dictionary.Start_Iteration();

  for (int x = 0; x < Dictionary.Number_of_Items(); x++)
    {
    TempHold = Dictionary.Get_Current();

    if (strcmp((char *)TempHold.Name, Name) == 0)
    {
      return &(TempHold.Value);
    }
    Dictionary.Next();
  }

  return 0;
}

void Read_ConfigFiles (const char *Path)  
{
  FILE *File;
  char *Buffer;
  String SBuffer;
  char *p;
  Entry TmpEntry;

  if ((File = fopen(Path, "r")) == 0)
    {
      return;
    }

  while (GetLine(SBuffer, File) !=  EOF)
    {
      Buffer = new char [SBuffer.Length() + 1];
      strcpy(Buffer, SBuffer);

      if (*Buffer != '#' && *Buffer != ' ' && *Buffer != '=')
	{
	  p = strtok(Buffer, "=");

	  if (p != 0)  
	    {
	      TmpEntry.Name = p;
	      p = strtok(0, "=");

	      if (p == 0)
		*TmpEntry.Value = '\0';
	      else
		TmpEntry.Value = p;

	      Register_Parameter(TmpEntry.Name, TmpEntry.Value);
	    }
	  delete [] Buffer;
	}	
    }
  
  fclose(File);
}


int GetLine(String &Buffer, FILE *File)
{
  int c;
  Buffer.Erase();

  c = fgetc(File);
  while (c != EOF && c != '\n')
    {
      Buffer.Append(c);
      c = fgetc(File);
    }

  Buffer.Append('\0');
  if (c == EOF)
    return EOF;
  else
    return Buffer.Length();
}

void Register_Parameter (const char *Name, const char *Value)
{
  Entry NewEntry;
  Entry New;
  int IsIn = 0;
  int x;

  Dictionary.Start_Iteration();

  for (x = 0; x < Dictionary.Number_of_Items(); x++)
  {
    NewEntry = Dictionary.Get_Current();

    if (strcmp((char *)NewEntry.Name, Name) == 0)
    {
      NewEntry.Value = Value;
      IsIn = 1;
      Dictionary.Get_Current() = NewEntry;
    }
    Dictionary.Next();
  }

  if (IsIn == 0)
  {
    New.Name = Name;
    New.Value = Value;
    Dictionary.Append(New);
  }
}










