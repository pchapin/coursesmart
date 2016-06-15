/*****************************************************************************
FILE          : MaterialsManager.cpp
LAST REVISION : 2004-11-12

*****************************************************************************/

#include "MaterialsManager.h"

MaterialsManager::MaterialsManager(FileSystem *f, const std::string &path)
  : fs(f), unit_path(path)
{
}

std::string MaterialsManager::getURL()
{
  std::string result;
  std::string path(unit_path);
  path += "/unit_index.url";

  int handle = fs->open(path.c_str(), FileSystem::Read);
  if (handle != -1) {
    fs->readLine(handle, result);
    fs->close(handle);
  }
  return result;
}

std::vector<std::string> MaterialsManager::getUploadList()
{
  return fs->getFileList(unit_path);
}

void MaterialsManager::addUploadFile(
  const std::string &file,
  const std::string &contents)
{
  fs->decode(unit_path + file, contents);
}

std::string MaterialsManager::getUploadFile(const std::string &file)
{
  return fs->encode(unit_path + file);
}

void MaterialsManager::deleteUploadFile(const std::string &file)
{
  fs->remove(unit_path + file);
}
