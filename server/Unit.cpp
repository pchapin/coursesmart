/*****************************************************************************
FILE          : Unit.cpp
LAST REVISION : 2004-11-30

*****************************************************************************/

#include "Unit.h"

Unit::Unit(FileSystem *fs, Logger* lg, const std::string &dir_name)
  : unitMaterials(fs, dir_name), unitUpload(fs, dir_name + "unit_uploads/")
{
  theFileSystem = fs;
  theLogger = lg;
}


std::vector<std::string>
  Unit::getUploadList(const std::string &userid, const std::string &user_role)
{
  return (user_role == "instructor") ?
    unitMaterials.getUploadList() : unitUpload.getUploadList(userid);
}


void Unit::addUploadFile(const std::string &userid,
                         const std::string &user_role,
                         const std::string &file,
                         const std::string &contents)
{
  if (user_role == "instructor")
    unitMaterials.addUploadFile(file, contents);
  else
    unitUpload.addUploadFile(userid, file, contents);
}


std::string
  Unit::getUploadFile(const std::string &userid,
                      const std::string &user_role,
                      const std::string &file)
{
  return (user_role == "instructor") ?
    unitMaterials.getUploadFile(file) : unitUpload.getUploadFile(userid, file);
}


void
  Unit::deleteUploadFile(const std::string &userid,
                         const std::string &user_role,
                         const std::string &file)
{
  if (user_role == "instructor")
    unitMaterials.deleteUploadFile(file);
  else
    unitUpload.deleteUploadFile(userid, file);
}
