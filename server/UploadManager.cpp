/*****************************************************************************
FILE          : UploadManager.cpp
LAST REVISION : 2004-11-22

*****************************************************************************/

#include "UploadManager.h"

UploadManager::UploadManager(FileSystem *f, const std::string &p)
  : fs(f), path(p)
{
}

std::vector<std::string> UploadManager::getUploadList(
  const std::string &userid)
{
  return fs->getFileList(path + userid);
}

void UploadManager::addUploadFile(
  const std::string &userid,
  const std::string &file,
  const std::string &contents)
{
  fs->mkdir(path);
  fs->mkdir(path + userid);
  fs->decode(path + userid + "/" + file, contents);
}

std::string UploadManager::getUploadFile(
  const std::string &userid, const std::string &file)
{
  return fs->encode(path + userid + "/" + file);
}

void UploadManager::deleteUploadFile(
  const std::string &userid, const std::string &file)
{
  fs->remove(path + userid + "/" + file);
}
