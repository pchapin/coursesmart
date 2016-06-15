/*****************************************************************************
FILE          : Course.cpp
LAST REVISION : 2004-11-27

The course object manages its list of units by caching any units it
creates in a map. More specifically, it stores pointers to dynamically
allocated units and indexes them by the unit directory names. Whenever
the caller asks for a new unit, the corresponding Unit object is
returned from cache or created if necessary. The caller should never
delete the Unit object. That is done instead in the destructor of the
Coure object. As a consequence of this, Unit objects, once created, live
as long as the client is connected.
*****************************************************************************/

#include "Course.h"

Course::Course(const std::string &path)
{
  theFileSystem = new FileSystem(path);
  theLogger     = new Logger(theFileSystem, "csserve.log");
  theUserData   = new UserData(theLogger, theFileSystem);
  theDiscussion = new Discussion(theLogger, theFileSystem);
  theGradeBook  = new GradeBook(theLogger, theFileSystem);
  theLogger->write("CourseSmart server started");
}

Course::~Course()
{
  delete theDiscussion;
  delete theUserData;
  delete theLogger;
  delete theFileSystem;

  // Get rid of all the unit objects cached in the unit map.
  std::map<std::string, Unit *>::iterator p = theUnits.begin();
  while (p != theUnits.end()) {
    delete p->second;
    ++p;
  }
}

std::string Course:: getPath()
{
  return path;
}

Discussion *Course::getDiscussion()
{
  return theDiscussion;
}

UserData *Course::getUserData()
{
  return theUserData;
}

GradeBook *Course::getGradeBook()
{
  return theGradeBook;
}

Logger *Course::getLogger()
{
  return theLogger;
}

Unit *Course::getUnit(const std::string &id)
{
  if(theUnits.find(id) == theUnits.end()) {
    theUnits[id] =
      new Unit(theFileSystem, theLogger, std::string("units/") + id + "/");
  }
  
  return theUnits[id];
}

std::vector<Course::UnitInformation> Course::getUnitList()
{
  int handle;
  std::vector<UnitInformation> unit_list;
  UnitInformation current;
	
  theFileSystem->lock();
  handle = theFileSystem->open("units/unit_list.txt", FileSystem::Read);
  if (handle != -1) {
    std::string line;
    while (theFileSystem->readLine(handle, line)) {
      if (line == "") continue;
      std::string::size_type position = line.find('|');
      current.name = line.substr(0, position);
      current.id = line.substr(position + 1);
      unit_list.push_back(current);
    }
    theFileSystem->close(handle);
  }
  theFileSystem->unlock();

  return unit_list;
}
