/****************************************************************************
FILE          : str.h
LAST REVISION : February 1996
SUBJECT       : Interface to class String
PROGRAMMER    : Peter Chapin

CS-212 students must implement the interface defined here as
programming problem #1.

Please send comments or bug reports to

     Peter Chapin
     P.O. Box 317
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef STR_H
#define STR_H

#include <limits.h>  // Needed for the object-like macro INT_MAX.

// Objects of type String use dynamic memory allocation to hold the characters
//   of the string. Thus the strings grow and shrink as needed and on demand.
//   There is no arbitrary, static limit to the size of a string. This class
//   does *not* bother to check for out of memory conditions. (Refer to class
//   notes for an explaination of why that is ok here).

class String {

private:
  char *Workspace;

public:

  // -----------------------------------
  // Constructing and copying Strings...
  // -----------------------------------
  String();
    // Default constructor builds an empty string.

  String(const String &);
    // Construct a string and copy the given string into it.

  String(const char *);
    // Construct a string and copy the given "old style" string into it.

  String &operator=(const String &);
    // Assign a given string to 'this' string. Return a reference to this
    //   string.

  String &operator=(const char *);
    // Assign a given "old style" string to 'this' string. Return a reference
    //   to this string.

 ~String() { delete [] Workspace; }
    // Clean up the string by freeing any memory that has been allocated to
    //   hold the string.

  // -------------------
  // Access functions...
  // -------------------

  operator char *() { return Workspace; }
    // Return a pointer to the internal array of characters composing this
    //   string. THIS FUNCTION IS A PROBLEM! We'd like to live without it,
    //   but it's just too handy to do without. Refer to class notes for
    //   more information.

  int Length();
    // Return the number of characters in this string. (Note that the size
    //   of the strings this class can handle are implicitly limited by the
    //   range of int.

  // -------------
  // Operations...
  // -------------

  void Append(const String &);
  void Append(const char *);
  void Append(char);
    // These functions append the given string, "old style" string, or char
    //   to the end of this string.

  void Erase();
    // Deletes all the characters in 'this' string so that it empty.

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // The following members are optional. You don't need to implement them
  //   to satisfy the requirements of programming problem #1. However, you
  //   might want to anyway because they are nice to have. These functions
  //   are based on the string handling features of the scripting language
  //   Rexx. If you implement all of these functions, you will have a
  //   String class as powerful as the Rexx built-in string type.
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  void Right(int Length, char Pad = ' ');
    // Replace this string with its rightmost 'Length' characters. If
    //   'Length' is greater than the length of this string, then
    //   extend this string (on the left) with 'Pad' characters as
    //   appropriate.

  void Left(int Length, char Pad = ' ');
    // Replace this string with its leftmost 'Length' characters in a
    //   fashion similar to the way Right() works.

  void Center(int Length, char Pad = ' ');
    // Center this string between runs of 'Pad' characters so that it is
    //   'Length' characters long. If this string is greater than 'Length'
    //   characters long, then truncate it on the right.

  int operator==(const String &);
    // Return 1 if the given string is the same as this string; 0 otherwise.

  int operator!=(const String &);
    // Return 0 if the given string is the same as this string; 1 otherwise.

  void Copy(int Count);
    // Replace this string with several concatenated copies of itself. The
    //   number of copies desired is in 'Count.' For example if this string
    //   was "abc," then a call to Copy(3) should cause this string to
    //   become "abcabcabc."

  void Delete(int Offset, int Count = INT_MAX);
    // Delete the substring starting at position 'Offset' (beginning of the
    //   string is offset zero) and of length 'Count.' If 'Offset' is off the
    //   end of the string, there is no effect. If 'Count' is so large that it
    //   would go off the end of the string, then the entire tail portion of
    //   the string is deleted. Should negative 'Offset' and 'Count' values be
    //   supported?

  void Insert(const String &Incoming, int Offset = -1, int Length = INT_MAX);
    // Insert the string 'Incoming' into this string just after position 'Offset.'
    //   Insert 'Length' characters of the incoming string (or all of them if
    //   'Length' is too long). 

  int Pos(char Needle, int Offset = 0);
    // Search this string forwards, starting at offset 'Offset' looking for
    //   the character 'Needle.' Return the offset of the character if found
    //   or -1 if it is not found.

  int Last_Pos(char Needle, int Offset = INT_MAX);
    // Search this string backwards, starting at offset 'Offset' (or at the
    //   end if 'Offset' is too large) looking for the character 'Needle.'
    //   Return the offset of the character if found or -1 if it is not found.

  void Strip(char Mode = 'B', char Kill_Char = ' ');
    // Removes leading or trailing (or both) instances of 'Kill_Char' from
    //   this string. Use a mode of 'L' to strip leading characters, 'T' to
    //   strip trailing characters, or 'B' to indicate both.

  void Substr(int Offset, int Count = INT_MAX);
    // Replace this string with a substring of itself. The substring starts
    //   at 'Offset' and is of length 'Count.'

// +++++
// The following member functions do operations on words inside the
//   string. Words are delimited by white space defined as spaces, tabs,
//   newlines, carriage returns, and formfeeds.
// +++++

  void Subword(int Offset, int Count = INT_MAX);
    // Replace this string with a substring consisting of 'Count' words
    //   starting at word number 'Offset.' The first word in the
    //   string is number zero. Embeded delimiter characters are
    //   retained exactly as they exist in the string, but leading
    //   and trailing delimiter characters are removed.

  void Word(int Offset) { Subword(Offset, 1); }
    // Replace this string with the nth word in the string.

  int Words();
    // Returns the number of words in this string.
};

#endif
