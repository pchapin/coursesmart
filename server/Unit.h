/*****************************************************************************
FILE          : Unit.h
LAST REVISION : 2004-11-30

This class manages a single unit. It provides access to the unit's
materials and the unit's "upload area" where students can submit their
answers to any unit related assignments. Currently this class is largely
a shell around classes that deal with the two aspects of a unit
mentioned above.
*****************************************************************************/

#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "FileSystem.h"
#include "Logger.h"
#include "MaterialsManager.h"
#include "UploadManager.h"

class Unit {
public:
  Unit(FileSystem *fs, Logger* lg, const std::string &dir_name);

  std::string
    getURL()
    { return unitMaterials.getURL(); }

  std::vector<std::string>
    getUploadList(const std::string &userid, const std::string &user_role);

  void addUploadFile(const std::string &userid,
                     const std::string &user_role,
                     const std::string &file,
                     const std::string &contents);
  
  std::string getUploadFile(const std::string &userid,
                            const std::string &user_role,
                            const std::string &file);

  void deleteUploadFile(const std::string &userid,
                        const std::string &user_role,
                        const std::string &file);

private:
  FileSystem      *theFileSystem;
  Logger          *theLogger;
  MaterialsManager unitMaterials;
  UploadManager	   unitUpload;
};

#endif
