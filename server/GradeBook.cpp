/*****************************************************************************
FILE          : GradeBook.cpp
LAST REVISION : 2004-11-28

*****************************************************************************/

#include "GradeBook.h"

//
// Split a string on '|' delimiters.
//
static std::vector<std::string> split(const std::string &buffer)
{
  std::vector<std::string> result;

  std::string::size_type start_pos, end_pos;

  start_pos = 0;
  while ((end_pos = buffer.find('|', start_pos)) != std::string::npos) {
    result.push_back(buffer.substr(start_pos, end_pos - start_pos));
    start_pos = end_pos + 1;
  }
  result.push_back(buffer.substr(start_pos));

  return result;
}


//
// Return true if the line is blank.
//
static bool is_blank(const std::string &line)
{
  for (std::string::size_type i = 0; i < line.size(); ++i) {
    if (line[i] != ' ' && line[i] != '\t') return false;
  }
  return true;
}


// ==============
// Public Methods
// ==============

GradeBook::GradeBook(Logger *lg, FileSystem *fs)
  : theFileSystem(fs), theLogger(lg),
    index_path("assignments.txt"),
    grades_path("grades.txt")
{
}

GradeBook::~GradeBook()
{
}

std::string GradeBook::getStudentGrades(const std::string &username)
{
	//returns just the number grades of username

	std::string line;
	int position = -1; //position of username found in line
	int handle_in;

	theFileSystem->lock();
	handle_in = theFileSystem->open(grades_path, FileSystem::Read);
		
	while(theFileSystem->readLine(handle_in, line))
	{
		position = line.find(username, 0);
		if(position > 0)
		{
			line.erase(0, position);
			theFileSystem->close(handle_in);
			theFileSystem->unlock();
			return line;
		}
	}
		
	line = "user not found";
	theFileSystem->close(handle_in);
	theFileSystem->unlock();
	return line;
}

std::vector<GradeBook::GradeInfo> GradeBook::getAllGrades()
{
  std::vector<GradeInfo> students;
  int handle_in;
  std::string::size_type pos;

  theFileSystem->lock();
  handle_in = theFileSystem->open(grades_path, FileSystem::Read);
  while(theFileSystem->readLine(handle_in, line)) {
    if (is_blank(line)) continue;  // More checking would be nice.

    GradeInfo student_grades;

    pos = line.find(' ');
    student_grades.name.assign(line, 0, pos);
    student_grades.grades.assign(line, pos + 1, std::string::npos);
    students.push_back(student_grades);
  }
		
  theFileSystem->close(handle_in);
  theFileSystem->unlock();
  return students;
}


void GradeBook::putAllGrades(const std::vector<GradeInfo> &grades)
{
  int handle_out;
  std::vector<GradeInfo>::const_iterator p;
	
  theFileSystem->lock();
  handle_out = theFileSystem->open(grades_path, FileSystem::Write);
  for (p = grades.begin(); p != grades.end(); ++p) {
    theFileSystem->writeLine(handle_out, p->name + " " + p->grades);
  }
  theFileSystem->close(handle_out);
  theFileSystem->unlock();
}


// Todo: Update for new assignment file format.
void GradeBook::addAssignment(const std::string &assignment)
{
	int handle_in, handle_out, handle_append;
	int length;

	//writes assignment name to index file
	theFileSystem->lock();
	handle_append = theFileSystem->open(index_path, FileSystem::Append);
	
	theFileSystem->writeLine(handle_append, assignment);

	theFileSystem->close(handle_append);
	
	//puts a place holder for all students in grades
	handle_in = theFileSystem->open(grades_path, FileSystem::Read);
	handle_out = theFileSystem->open("temp", FileSystem::Write);
	
	while(theFileSystem->readLine(handle_in, line))
	{
		length = line.length();
		line.append(" -", length, 1);
		theFileSystem->writeLine(handle_out, line);
	}

	theFileSystem->close(handle_in);
	theFileSystem->close(handle_out);

	if(theFileSystem->remove(grades_path))
		theLogger->write("removed "+grades_path);
	
	if(theFileSystem->rename("temp", grades_path));
		theLogger->write("renamed temp to "+ grades_path);

	theFileSystem->unlock();
}


// Todo: Update for new assignment file format.
void GradeBook::deleteAssignment(const std::string &assignment)
{
  int handle_in, handle_out;		  // Handles for filesystem
  int line_count = 0, deleted_line = -1;  // Counters for deleted assignment
  int space_count = 0, next_space = -1;	  // Counters for spaces and position of next space
  int j = -1, k;

	//deletes assignment in index file
	theFileSystem->lock();
	handle_in = theFileSystem->open(index_path, FileSystem::Read);
	handle_out = theFileSystem->open("temp", FileSystem::Write);

	while(theFileSystem->readLine(handle_in, line))
	{
		if(line != assignment)
		{
			line_count++;
			theFileSystem->writeLine(handle_out, line);
		}

		if(line == assignment)
		{
			deleted_line = line_count + 1; 
		}
	}

	theFileSystem->close(handle_out);
	theFileSystem->close(handle_in);
	
	if(theFileSystem->remove(index_path))
		theLogger->write("removed "+index_path);
	
	if(theFileSystem->rename("temp", index_path));
		theLogger->write("renamed temp to "+ index_path);

	handle_in = theFileSystem->open(grades_path, FileSystem::Read);
	handle_out = theFileSystem->open("temp", FileSystem::Write);

	while(theFileSystem->readLine(handle_in, line))
	{
		for(unsigned i = 0; i < line.length(); i++) //fix warning
		{
			if(line[i] == ' ')
			{
				space_count++;
			}
			if(space_count == deleted_line)
			{
				j = i + 1;
				next_space = line.find(' ', j);
				k = next_space - j;
				line.erase(j, k);
			}
		}

		theFileSystem->writeLine(handle_out, line);
	}
	
	theFileSystem->close(handle_in);
	theFileSystem->close(handle_out);

	if(theFileSystem->remove(grades_path))
		theLogger->write("removed "+grades_path);
	
	if(theFileSystem->rename("temp", grades_path));
		theLogger->write("renamed temp to "+ grades_path);

	theFileSystem->unlock();
}

std::vector<GradeBook::AssignmentInfo> GradeBook::getAssignments()
{
  int handle_in;

  std::vector<AssignmentInfo> assignments;

  theFileSystem->lock();
  handle_in = theFileSystem->open(index_path, FileSystem::Read);

  while(theFileSystem->readLine(handle_in, line)) {
    if (is_blank(line)) continue;  // More checking would be nice.

    AssignmentInfo new_assignment;
    std::vector<std::string> fields = split(line);

    new_assignment.name = fields[0];
    new_assignment.id = fields[1];
    assignments.push_back(new_assignment);
  }
	
  theFileSystem->close(handle_in);
  theFileSystem->unlock();

  return assignments;
}
