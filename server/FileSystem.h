/*****************************************************************************
FILE          : FileSystem.h
LAST REVISION : 2004-12-02

Class FileSystem represents an abstraction of a file system. It provides
methods for opening, reading, writing, and closing files along with some
other operations. Many of FileSystem's methods simply forward to the
underlying operating system. Some methods, however, add functionality or
translate the request in some (hopefully) useful way.

Clients of FileSystem can expect uniform semantics [Todo: document these
semantics more precisely] regardless of the underlying implementation. A
future version of FileSystem might be abstract with concrete classes for
handling several different types of file systems.
*****************************************************************************/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iosfwd>
#include <string>
#include <vector>

class FileSystem {
public:
  enum mode_type { Read, Write, Append };
  typedef std::vector<std::string> namelist_type;

  FileSystem(const std::string &path);
 ~FileSystem();

  int           open(const std::string &name, mode_type mode);
  void          close(int);
  bool          readLine(int handle, std::string &line);
  bool          writeLine(int handle, const std::string &line);
  void          flush(int handle);
  bool          remove(const std::string &name);
  bool          rename(const std::string &oldname, const std::string &newname);
  bool          mkdir(const std::string &name);
  bool          rmdir(const std::string &name);
  namelist_type getFileList(const std::string &dir_name);
  namelist_type getDirList(const std::string &dir_name);
  std::string   encode(const std::string &name);
  void          decode(const std::string &name, const std::string &contents);
  std::string   getRoot();

  void          lock();
  void          unlock();

private:
  typedef std::vector<std::iostream *> filetable_type;

  std::string    system_root;
  filetable_type file_table;
  int            semid;

  // Makes a local path into a global path. Leading slash on 'path' optional.
  std::string normalize_path(const std::string &path);

  // Verifies that handle is in bounds and refers to a valid file.
  bool verify_handle(int handle);

  // Get list of items in the named directory. Flag == 0 for files, 1 for dirs.
  namelist_type FileSystem::get_list(const std::string &dir_name,
                                     int type_flag);

};

#endif
