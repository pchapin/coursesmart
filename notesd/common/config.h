/****************************************************************************
FILE          : config.h
LAST REVISION : March 1996
SUBJECT       : Configuration file manager.
PROGRAMMER    : 

Please send comments or bug reports to

     c/o Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@twilight.vtc.vsc.edu
****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include "str.h"

void Read_ConfigFiles(const char *Path);
  // Reads the configuration file named by 'Path' and processes all the
  //   (name, value) pairs it finds there. This function sets up a dictionary
  //   of (name, value) pairs where both 'name' and 'value' are simple text
  //   strings.

String *Lookup_Parameter(const char *Name);
  // Given a name return the value. The search of the dictionary is done
  //   in a case sensitive manner. This function returns 0 if the parameter
  //   is not in the dictionary.

void Register_Parameter(const char *Name, const char *Value);
  // Install a (name, value) pair into the dictionary. This function is
  //   useful for setting default values before calling Read_ConfigFiles().
  //   Any value found in a config file will override the corresponding
  //   value installed beforehand.

#endif
