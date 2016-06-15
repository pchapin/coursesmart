/*****************************************************************************
FILE          : UserData.h
LAST REVISION : 2004-11-20

This class represents the database of user information.
*****************************************************************************/

#ifndef USERDATA_H
#define USERDATA_H

#include <string>
#include <vector>
#include <fstream>

#include "FileSystem.h"
#include "Logger.h"

class UserData {
public:
  enum role_type { Nonexistent, Instructor, Grader, Student };

  struct UserInformation {
    std::string user_name;
    std::string last_name;
    std::string first_name;
    std::string role;
  };

  UserData(Logger *l, FileSystem *f) : lg(l), fs(f) { }

  // Add given user to database using given password. Return true if success.
  bool addUser(const std::string &uname,  // Username. Must be unique.
               const std::string &lname,  // Last name.
               const std::string &fname,  // First name.
               const std::string &pword,  // Password.
               role_type role);

  // Remove given user from database.
  void removeUser(const std::string &uname);

  // Check given user's password. Return role in string form if success.
  // Return the special role "nonexistent" if the username/password
  // don't match any users in the database.
  // 
  std::string verifyUser(const std::string &uname,  // IN : given user name.
                         const std::string &pword,  // IN : given password.
                         std::string *l_name,       // OUT: user's last name.
                         std::string *f_name);      // OUT: user's first name.
  
  // Return list of all user names in database.
  std::vector<UserInformation> getUserList();

private:
  Logger     *lg;
  FileSystem *fs;
};

#endif
