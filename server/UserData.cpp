/*****************************************************************************
FILE          : UserData.cpp
LAST REVISION : 2004-11-20

The database file is formatted as follows: Each user is described on a
single line. The fields on that line are '|' separated. The username
is first followed by the password and then the role list. Currently each
user is only expected to be in a single role.

*****************************************************************************/

#include <sstream>

#include "UserData.h"

//
// Convert a role into a string.
//
static std::string get_role_string(UserData::role_type role)
{
  static char *role_names[] = {
    "nonexistent", "instructor", "grader", "student"
  };

  return std::string(role_names[role]);
}

//
// Case insensitive string comparison. Return true if equal.
//
static bool ci_compare(const char *s1, const char *s2)
{
  if (std::strlen(s1) != std::strlen(s2)) return false;

  while (*s1) {
    if (std::toupper(*s1) != std::toupper(*s2)) return false;
    ++s1; ++s2;
  }
  return true;
}

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

bool UserData::addUser(const std::string &uname,
                       const std::string &lname,
                       const std::string &fname,
                       const std::string &pword,
                       role_type role)
{
  int in_handle, out_handle;
  std::string line;
  std::string new_info;
  std::stringstream formatter;
  bool update_complete = false;

  formatter << "User " << uname;

  // Construct a line containing the new information.
  new_info = uname + '|' + lname + '|' + fname +
                     '|' + pword +
                     '|' + get_role_string(role);

  // Install the new information into the user database file.
  fs->lock();
  in_handle=fs->open("users.txt", FileSystem::Read);
  out_handle=fs->open("temp.txt", FileSystem::Write);
  while (fs->readLine(in_handle, line)) {
    if (is_blank(line)) continue;

    std::string::size_type position = line.find('|');
    if(line.substr(0, position) == uname) {
      fs->writeLine(out_handle, new_info);
      update_complete = true;
      formatter << " information updated";
      lg->write(formatter.str());
    }
    else {
      fs->writeLine(out_handle, line);
    }
  }
  if (!update_complete) {
    fs->writeLine(out_handle, new_info);
    formatter << " added";
    lg->write(formatter.str());
  }
  fs->close(in_handle);
  fs->close(out_handle);
  fs->remove("users.txt");
  fs->rename("temp.txt", "users.txt");
  fs->unlock();
  return true;
}


void UserData::removeUser(const std::string &uname)
{
  int in_handle, out_handle;
  std::string line;
  std::stringstream formatter;

  formatter << "User " << uname << " removed";

  fs->lock();
  in_handle = fs->open("users.txt", FileSystem::Read);
  out_handle = fs->open("temp.txt", FileSystem::Write);
  while(fs->readLine(in_handle, line)) {
    if (is_blank(line)) continue;

    std::string::size_type position = line.find('|');
    if(line.substr(0, position) != uname)
      fs->writeLine(out_handle, line);
  }
  fs->close(in_handle);
  fs->close(out_handle);
  fs->remove("users.txt");
  fs->rename("temp.txt", "users.txt");
  lg->write(formatter.str());
  fs->unlock();
}


std::string UserData::verifyUser(const std::string &uname,
                                 const std::string &pword,
                                 std::string *l_name,
                                 std::string *f_name)
{
  std::string line;
  std::string result(get_role_string(Nonexistent));
  int handle;

  fs->lock();
  handle = fs->open("users.txt", FileSystem::Read);
  while(fs->readLine(handle, line)) {
    if (is_blank(line)) continue;

    std::vector<std::string> fields(split(line));

    if(ci_compare(fields[0].c_str(), uname.c_str()) && (fields[3] == pword)) {
      if (l_name) *l_name = fields[1];
      if (f_name) *f_name = fields[2];
      result = fields[4];
      break;
    }
  }
  fs->close(handle);
  fs->unlock();
  return result;
}


std::vector<UserData::UserInformation> UserData::getUserList()
{
  std::string line;
  std::vector<UserInformation> users;
  int handle;

  fs->lock();
  handle = fs->open("users.txt", FileSystem::Read);
  while(fs->readLine(handle, line)) {
    if (is_blank(line)) continue;

    std::vector<std::string> fields(split(line));

    UserInformation user = { fields[0], fields[1], fields[2], fields[4] };
    users.push_back(user);
  }
  fs->close(handle);
  fs->unlock();
  return users;
}
