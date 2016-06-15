/****************************************************************************
FILE          : log.h
LAST REVISION : March 1996
SUBJECT       : Log file management functions.
PROGRAMMER    : 

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef LOG_H
#define LOG_H

int Open_Log();
  // This function opens the log file. It returns 0 if it fails, 1 otherwise.

void Close_Log();
  // This function closes the log file.

void Write_Log(char *Format, ...);
  // This function writes a line to the log file. It accepts a printf style
  //   format string (with additional parameters as necessary). Each line
  //   in the log file specifies the date/time of the event as well as
  //   the message above.

#endif

