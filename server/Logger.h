/*****************************************************************************
FILE          : Logger.h
LAST REVISION : 2004-11-02

Do we want to define a standard header for our source files?

*****************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "FileSystem.h"

class Logger {
public:
  Logger(FileSystem *f, const std::string &name);
 ~Logger();

  void write(const std::string &message);

private:
  FileSystem *fs;
  int         logfile_handle;
};

#endif
