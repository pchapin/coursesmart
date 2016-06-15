/*****************************************************************************
FILE          : Course.h
LAST REVISION : 2004-11-27

The Course class is a container that holds the various components of the
course in a single, easy to find location. This class is also
responsible for managing the list of units and concerns itself with the
mapping between human readible unit names and their locations in the
file system.
*****************************************************************************/

#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <map>

#include "Discussion.h"
#include "FileSystem.h"
#include "GradeBook.h"
#include "Logger.h"
#include "Unit.h"
#include "UserData.h"

class Course {
public:
  Course(const std::string &path);
 ~Course();

  struct UnitInformation {
    std::string name;   // For human consumption; need not be unique.
    std::string id;     // ID number is same as unit directory name.
  };

  std::string getPath();
  Discussion *getDiscussion();
  UserData   *getUserData();
  Logger     *getLogger();
  Unit	     *getUnit(const std::string &id);
  std::vector<UnitInformation> getUnitList();
  GradeBook  *getGradeBook();

private:
  std::string  path;
  Discussion  *theDiscussion;
  FileSystem  *theFileSystem;
  UserData    *theUserData;
  Logger      *theLogger;
  std::map<std::string, Unit*> theUnits;
  GradeBook   *theGradeBook;
};

extern Course* theCourse;

#endif
