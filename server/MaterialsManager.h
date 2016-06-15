/*****************************************************************************
FILE          : MaterialsManager.h
LAST REVISION : 2004-11-30

This class manages the materials (basically a web site) of a single
course unit.
*****************************************************************************/

#ifndef MATERIALSMANAGER_H
#define MATERIALSMANAGER_H

#include <string>
#include "FileSystem.h"

class MaterialsManager {
public:
  MaterialsManager(FileSystem *f, const std::string &path);

  std::string getURL();

  std::vector<std::string> getUploadList();
  void addUploadFile(const std::string &file, const std::string &contents);
  std::string getUploadFile(const std::string &file);
  void deleteUploadFile(const std::string &file);

private:
  FileSystem *fs;
  std::string unit_path;
};

#endif
