/*****************************************************************************
FILE          : Discussion.cpp
LAST REVISION : 2004-12-02

*****************************************************************************/

#include <cstdio>
#include <sstream>
#include "Discussion.h"

Discussion::Discussion(Logger *l, FileSystem *f)
  : lg(l), fs(f), path("discussion/")
  { }

std::string Discussion::getMessage(int id)
{
  int handle;
  std::stringstream formatter;
  std::string line;
  std::string result;

  formatter << id << ".msg";
  fs->lock();
  handle = fs->open(path + formatter.str(), FileSystem::Read);
  while (fs->readLine(handle, line)) {
    result += line;
    result += "\n";
  }
  fs->close(handle);
  fs->unlock();

  return result;
}

void Discussion::deleteMessage(int id)
{
  std::stringstream formatter;

  formatter << id << ".msg";
  fs->lock();
  fs->remove(path + formatter.str());
  fs->unlock();

  // Invoke Mhonarc to remove the message from the index.
  std::stringstream cmd_formatter;
  cmd_formatter << "mhonarc";
  cmd_formatter << " -outdir " << (fs->getRoot() + "/units/discussion");
  cmd_formatter << " -rmm";
  cmd_formatter << " " << id;

  std::system(cmd_formatter.str().c_str());
}


int Discussion::addMessage(std::string message)
{
  int handle;
  int id;

  fs->lock();

  // Get current count (id on next message).
  std::string line;
  handle = fs->open("message_count.txt", FileSystem::Read);
  fs->readLine(handle, line);
  fs->close(handle);
  id = std::atoi(line.c_str());

  // Write updated count (id for next message).
  std::stringstream id_formatter;
  id_formatter << id + 1;
  handle = fs->open("message_count.txt", FileSystem::Write);
  fs->writeLine(handle, id_formatter.str());
  fs->close(handle);

  // Write actual message.
  std::stringstream name_formatter;
  name_formatter << id << ".msg";
  handle = fs->open(path + name_formatter.str(), FileSystem::Write);
  fs->writeLine(handle, message);
  fs->close(handle);

  fs->unlock();

  // Invoke Mhonarc to compute the HTML version of the message.
  std::stringstream cmd_formatter;
  cmd_formatter << "mhonarc";
  cmd_formatter << " -outdir " << (fs->getRoot() + "/units/discussion");
  cmd_formatter << " -add";
  cmd_formatter << " < " << (fs->getRoot() + "/discussion/") << id << ".msg"; 
  // Should we redirect the I/O on this? Probably.

  std::system(cmd_formatter.str().c_str());

  return id;
}

