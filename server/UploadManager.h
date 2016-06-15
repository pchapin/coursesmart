/*****************************************************************************
FILE          : UploadManager.h
LAST REVISION : 2004-11-21

This class manages the upload area (for assignment submissions) for a
single course unit.
*****************************************************************************/

#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include <string>
#include <vector>

#include "FileSystem.h"

class UploadManager {
public:
  UploadManager(FileSystem *f, const std::string &p);

  std::vector<std::string>
  getUploadList(const std::string &userid);

  void
  addUploadFile(const std::string &userid,
                const std::string &file,
                const std::string &contents);

  std::string
  getUploadFile(const std::string &userid, const std::string &file);

  void
  deleteUploadFile(const std::string &userid, const std::string &file);

private:
  FileSystem *fs;
  std::string path;
};

#endif
