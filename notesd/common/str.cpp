/****************************************************************************
FILE          : str.C
LAST REVISION : February 1996
SUBJECT       : The implementation of a String class.
PROGRAMMER    : Peter Chapin

This code is public domain. It is an extended solution to the CS-212
programming problem #1 (Spring, 1996).

Please send comments or bug reports to

     Peter Chapin
     P.O. Box 317
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "str.h"

//-------------------------------------------------
//           Internally Linked Functions
//-------------------------------------------------

static int Is_White(int Ch)
{
  if (Ch == ' ' || Ch == '\t' || Ch == '\r' || Ch == '\n' || Ch == '\f') return 1;
  return 0;
}

//--------------------------------------
//           Member Functions
//--------------------------------------


String::String()
{
   Workspace = new char[1];
  *Workspace = '\0';
}


String::String(const String &Existing)
{
  Workspace = new char[strlen(Existing.Workspace) + 1];
  strcpy(Workspace, Existing.Workspace);
}


String::String(const char *Existing)
{
  Workspace = new char[strlen(Existing) + 1];
  strcpy(Workspace, Existing);
}


String &String::operator=(const String &Other)
{
  delete [] Workspace;
  Workspace = new char[strlen(Other.Workspace) + 1];
  strcpy(Workspace, Other.Workspace);
  return *this;
}


String &String::operator=(const char *Other)
{
  delete [] Workspace;
  Workspace = new char[strlen(Other) + 1];
  strcpy(Workspace, Other);
  return *this;
}


int String::Length()
{
  return strlen(Workspace);
}


void String::Append(const String &Other)
{
  char *Temp = new char[strlen(Workspace) + strlen(Other.Workspace) + 1];
  strcpy(Temp, Workspace);
  strcat(Temp, Other.Workspace);
  delete [] Workspace;
  Workspace = Temp;
}


void String::Append(const char *Other)
{
  char *Temp = new char[strlen(Workspace) + strlen(Other) + 1];
  strcpy(Temp, Workspace);
  strcat(Temp, Other);
  delete [] Workspace;
  Workspace = Temp;
}


void String::Append(char Other)
{
  int Size = strlen(Workspace) + 1;
  char *Temp = new char[Size + 1];
  strcpy(Temp, Workspace);
  Temp[Size - 1] = Other;
  Temp[Size    ] = '\0';
  delete [] Workspace;
  Workspace = Temp;
}


void String::Erase()
{
   delete [] Workspace;
   Workspace = new char[1];
  *Workspace = '\0';
}


void String::Right(int Length, char Pad)
{
  // Ignore attempts to use a negative count.
  if (Length < 0) return;

  int Current_Length = strlen(Workspace);

  // If this string is the right size, just return.
  if (Current_Length = Length) return;
  
  // If we need to make the string shorter...
  if (Length < Current_Length) {
    char *Temp = new char[Length + 1];
    strcpy(Temp, &Workspace[Current_Length - Length]);
    delete [] Workspace;
    Workspace = Temp;
  }

  // otherwise we need to make the string longer...
  else {
    char *Temp = new char[Length + 1];
    memset(Temp, Pad, Length - Current_Length);
    strcpy(&Temp[Length - Current_Length], Workspace);
    delete [] Workspace;
    Workspace = Temp;
  }
}


void String::Left(int Length, char Pad)
{
  // Ignore attempts to use a negative count.
  if (Length < 0) return;

  int Current_Length = strlen(Workspace);

  // If this string is the right size, just return.
  if (Current_Length == Length) return;

  // If we need to make the string shorter...
  if (Length < Current_Length) {
    char *Temp = new char[Length + 1];
    strncpy(Temp, Workspace, Length);
    Temp[Length] = '\0';
    delete [] Workspace;
    Workspace = Temp;
  }

  // otherwise we need to make the string longer...
  else {
    char *Temp = new char[Length + 1];
    strcpy(Temp, Workspace);
    memset(&Temp[Current_Length], Pad, Length - Current_Length);
    Temp[Length] = '\0';
    delete [] Workspace;
    Workspace = Temp;
  }

}


void String::Center(int Length, char Pad)
{
  // Ignore attempts to use a negative length.
  if (Length < 0) return;

  int Current_Length = strlen(Workspace);

  // If the current string is too small...
  if (Current_Length < Length) {
    int Difference = Length - Current_Length;
    int Left_Side  = Difference/2;
    int Right_Side = Length - Left_Side - Current_Length;
    char *Temp = new char[Length + 1];
    memset(Temp, Pad, Left_Side);
    strcpy(&Temp[Left_Side], Workspace);
    memset(&Temp[Left_Side + Current_Length], Pad, Right_Side);
    Temp[Length] = '\0';
    delete [] Workspace;
    Workspace = Temp;
  }

  // Otherwise, it's just a Left() operation.
  else {
    Left(Length, Pad);
  }
}


int String::operator==(const String &The_Other)
{
  return (strcmp(Workspace, The_Other.Workspace) == 0);
}


int String::operator!=(const String &The_Other)
{
  return (strcmp(Workspace, The_Other.Workspace) != 0);
}


void String::Copy(int Count)
{
  // Ignore attempts to use a negative count.
  if (Count < 0) return;

  char *Temp = new char[Count*strlen(Workspace) + 1];

  Temp[0] = '\0';
  for (int i = 0; i < Count; i++) strcat(Temp, Workspace);
  delete [] Workspace;
  Workspace = Temp;
}


void String::Delete(int Offset, int Count)
{
  // Ignore negative parameters.
  if (Offset < 0 || Count < 0) return;

  int Current_Length = strlen(Workspace);

  // Verify that there is actual work to do.
  if (Offset > Current_Length || Count == 0) return;

  // Trim the count so that it fits into the bounds on the string.
  if (Offset + Count > Current_Length) Count -= Current_Length - Offset - 1;

  // Now do the work.
  char *Temp = new char[Current_Length - Count + 1];
  memcpy(Temp, Workspace, Offset);
  memcpy(&Temp[Offset], &Workspace[Offset + Count], Current_Length - (Offset + Count));
  delete [] Workspace;
  Workspace = Temp;
}


void String::Insert(const String &Incoming, int Offset, int Length)
{
  // Insert the string 'Incoming' into this string just after position 'Offset.'
  //   Insert 'Length' characters of the incoming string (or all of them if
  //   'Length' is too long).
  fprintf(stderr, "\nString::Insert() called but not implemented!\n");
}


int String::Pos(char Needle, int Offset)
{
  // If we are starting off the end of the string, then obviously we didn't
  //   find anything. Note that this function *does* allow the caller
  //   to locate the null character at the end of the string.
  if (Offset < 0 || Offset > strlen(Workspace)) return -1;

  // Locate the character.
  char *p = Workspace + Offset;
  p = strchr(p, Needle);

  // If we didn't find it, return error.
  if (p == 0) return -1;

  // Otherwise return the offset to the character.
  return (int)(p - Workspace);
}


int String::Last_Pos(char Needle, int Offset)
{
  // Handle the case of Offset being off the end of the string.
  if (Offset < 0) return -1;
  if (Offset > strlen(Workspace)) Offset = strlen(Workspace);

  char *p = Workspace + Offset;

  // Now back up. If we find the character, return the offset to it.
  while (p >= Workspace) {
    if (*p == Needle) return (int)(p - Workspace);
    p--;
  }

  // If we got here, then we didn't find the character.
  return -1;
}


void String::Strip(char Mode, char Kill_Char)
{
    // Removes leading or trailing (or both) instances of 'Kill_Char' from
    //   this string. Use a mode of 'L' to strip leading characters, 'T' to
    //   strip trailing characters, or 'B' to indicate both.
  fprintf(stderr, "\nString::Strip() called but not implemented!\n");
}


void String::Substr(int Offset, int Count)
{
    // Replace this string with a substring of itself. The substring starts
    //   at 'Offset' and is of length 'Count.'
  fprintf(stderr, "\nString::Substr() called but not implemented!\n");
}


void String::Subword(int Offset, int Count)
{
    // Replace this string with a substring consisting of 'Count' words
    //   starting at word number 'Offset.' The first word in the
    //   string is number zero. Embeded delimiter characters are
    //   retained exactly as they exist in the string, but leading
    //   and trailing delimiter characters are removed.
  fprintf(stderr, "\nString::Subword() called but not implemented\n");
}


int String::Words()
{
  int Word_Count = 0;  // The number of words found.
  int In_Word    = 0;  // =1 When we are scanning a word.

  // Scan down the string...
  for (char *p = Workspace; *p; p++) {

    // If this is the start of a word...
    if (!Is_White(*p) && !In_Word) {
      Word_Count++;
      In_Word = 1;
    }

    // If we just finished scanning a word...
    if (Is_White(*p) && In_Word) {
      In_Word = 0;
    }
  }

  return Word_Count;
}

