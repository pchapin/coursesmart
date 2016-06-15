/*****************************************************************************
FILE          : Discussion.h
LAST REVISION : 2004-11-29

This class manages a single discussion board.
*****************************************************************************/

#ifndef DISCUSSION_H
#define DISCUSSION_H

#include <string>

#include "Logger.h"
#include "FileSystem.h"

class Discussion {
public:
  Discussion(Logger *l, FileSystem *f);

  std::string getMessage(int id);
  void        deleteMessage(int id);
  int         addMessage(std::string message);

private:
  Logger     *lg;
  FileSystem *fs;

  std::string path;
};

#endif
