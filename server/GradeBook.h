/*****************************************************************************
FILE          : GradeBook.h
LAST REVISION : 2004-11-27

This class manages the grade book of a single course.
*****************************************************************************/

#ifndef GRADEBOOK_H
#define GRADEBOOK_H

#include <string>
#include <vector>

#include "FileSystem.h"
#include "Logger.h"
#include "UserData.h"


class GradeBook {
public:

  struct GradeInfo {
    std::string name;
    std::string grades;
  }; 

  struct AssignmentInfo {
    std::string name;
    std::string id;
  };

  GradeBook(Logger *lg, FileSystem *fs);
 ~GradeBook();

  std::string getStudentGrades(const std::string &username);
  std::vector<GradeInfo> getAllGrades();
  void putAllGrades(const std::vector<GradeInfo> &grades);
  void addAssignment(const std::string &assignment);
  void deleteAssignment(const std::string &assignment);
  std::vector<AssignmentInfo> getAssignments();

private:
  FileSystem *theFileSystem;
  Logger     *theLogger;
  std::string line;
  std::string index_path;	// Index of assignments
  std::string grades_path;	// Grades and student usernames
};

#endif
