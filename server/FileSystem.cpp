/*****************************************************************************
FILE          : FileSystem.cpp
LAST REVISION : 2004-11-22

This implementation uses the Standard C++ library for file handling and
the POSIX API for operations not provided by Standard C++.
*****************************************************************************/

// C++
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>

// POSIX
#include <dirent.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>

// CourseSmart
#include "FileSystem.h"

// POSIX requires that I explictly declare this union.
union semun {
  int              val;
  struct semid_ds *buf;
  ushort          *array;
};

// Used for base64 encoding/decoding.
static const char code_table[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// ===============
// Private Methods
// ===============

std::string FileSystem::normalize_path(const std::string &path)
{
  // Let a null path represent the system's root directory.
  if (path.size() == 0) return system_root;

  // Remove occurances of ".." from path for security reasons?

  if (path[0] == '/') return system_root + path;
  return system_root + "/" + path;
}


bool FileSystem::verify_handle(int handle)
{
  // See comments in FileSystem::open().
  if (handle < 0 ||
      static_cast<filetable_type::size_type>(handle) >= file_table.size()) {
    return false;
  }
  if (file_table[handle] == NULL) return false;
  return true;
}


FileSystem::namelist_type FileSystem::get_list(const std::string &dir_name,
                                               int type_flag)
{
  namelist_type  result;
  std::string    path(normalize_path(dir_name));
  DIR           *dp;
  struct dirent *entry;
  struct stat    statbuf;
  
  // Scan the directory.
  if ((dp = opendir(path.c_str())) != NULL) {
    while ((entry = readdir(dp)) != NULL) {

      // If the current name is dot or dotdot, ignore it.
      if (strcmp(entry->d_name, "." ) == 0 ||
          strcmp(entry->d_name, "..") == 0) continue;

      // If the current name is that of a file, add it to the result.
      std::string full_name(path + "/" + entry->d_name);
      if (stat(full_name.c_str(), &statbuf) != -1) {
        if (type_flag == 0 && S_ISREG(statbuf.st_mode)) {
          result.push_back(entry->d_name);
        }
        if (type_flag == 1 && S_ISDIR(statbuf.st_mode)) {
          result.push_back(entry->d_name);
        }
      }
    }
    closedir(dp);
  }

  return result;
}


// ==============
// Public Methods
// ==============

FileSystem::FileSystem(const std::string &path) : system_root(path)
{
  // Sanitize system_root. Remove trailing slash (fails for root directory).
  if (system_root.size() == 0) system_root = ".";
  if (system_root[system_root.size() - 1] == '/')
    system_root.erase(system_root.size() - 1);
  // Add check for writability?

  // Create binary semaphore for use in locking.
  if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRUSR | S_IWUSR)) >= 0) {
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
  }
}


FileSystem::~FileSystem()
{
  filetable_type::iterator p;

  // Close all open files.
  for (p = file_table.begin(); p != file_table.end(); ++p) {
    delete *p;
  }

  // This should only be executed in the parent process.
  // semctl(semid, 0, IPC_RMID);
}


int FileSystem::open(const std::string &name, mode_type mode)
{
  int handle;
  std::string path(normalize_path(name));

  std::ios::openmode file_mode = std::ios::in;
  if (mode == Write) file_mode = std::ios::out;
  if (mode == Append) file_mode = std::ios::out|std::ios::app;

  std::fstream *file = new std::fstream(path.c_str(), file_mode);
  if (!*file) {
    delete file;
    return -1;
  }

  // Look for a free slot in the file table.
  std::fstream *search_value = NULL;
  filetable_type::iterator p =
    std::find(file_table.begin(), file_table.end(), search_value);
  if (p != file_table.end()) {
    *p = file;
    // Handle might not be representable as int.
    handle = p - file_table.begin();
  }
  else {
    file_table.push_back(file);  // Assume this succeeds.
    // Handle might not be representable as int.
    handle = file_table.size() - 1;
  }

  return handle;
}


void FileSystem::close(int handle)
{
  if (!verify_handle(handle)) return;

  delete file_table[handle];
  file_table[handle] = NULL;
}


bool FileSystem::readLine(int handle, std::string &line)
{
  if (!verify_handle(handle)) return false;

  std::getline(*file_table[handle], line);

  if (!*file_table[handle]) return false;
  return true;
}


bool FileSystem::writeLine(int handle, const std::string &line)
{
  if (!verify_handle(handle)) return false;

  *file_table[handle] << line << "\n";
  return true;
}


void FileSystem::flush(int handle)
{
  if (!verify_handle(handle)) return;

  file_table[handle]->flush();
}


bool FileSystem::remove(const std::string &name)
{
  std::string path(normalize_path(name));

  if (std::remove(path.c_str()) != 0) return false;
  return true;
}


bool FileSystem::rename(const std::string &oldname, const std::string &newname)
{
  std::string oldpath(normalize_path(oldname));
  std::string newpath(normalize_path(newname));

  if (std::rename(oldpath.c_str(), newpath.c_str()) != 0) return false;
  return true;
}


bool FileSystem::mkdir(const std::string &name)
{
  std::string path(normalize_path(name));

  if (::mkdir(path.c_str(), 0755) != 0) return false;
  return true;
}


bool FileSystem::rmdir(const std::string &name)
{
  std::string path(normalize_path(name));

  if (::rmdir(path.c_str()) != 0) return false;
  return true;
}


FileSystem::namelist_type FileSystem::getFileList(const std::string &dir_name)
{
  return get_list(dir_name, 0);
}


FileSystem::namelist_type FileSystem::getDirList(const std::string &dir_name)
{
  return get_list(dir_name, 1);
}


std::string FileSystem::encode(const std::string &name)
{
  std::string path(normalize_path(name));
  std::FILE *infile = std::fopen(path.c_str(), "rb");
  if (infile == NULL) return "";

      std::string result;        // Place for the encoded results.
      std::size_t count;         // Number of bytes read from the input.
  unsigned char   in_buffer[3];  // Buffer for the decoded data.
  unsigned char   out_buffer[4]; // Buffer for the encoded data.
  unsigned int    index;         // Index into the code table.

  // Initialize.
  std::memset(in_buffer, 0, 3);
  std::memset(out_buffer, '=', 4);

  // Process the input file three bytes at a time.
  while ((count =
            std::fread(in_buffer, sizeof(unsigned char), 3, infile)) != 0) {

    // Handle the first input byte.
    index  = (in_buffer[0] & 0xFC) >> 2;
    out_buffer[0] = code_table[index];
    index  = (in_buffer[0] & 0x03) << 4;
    index |= (in_buffer[1] & 0xF0) >> 4;
    out_buffer[1] = code_table[index];

    if (count > 1) {

      // Finish handling the second input byte.
      index  = (in_buffer[1] & 0x0F) << 2;
      index |= (in_buffer[2] & 0xC0) >> 6;
      out_buffer[2] = code_table[index];

      if (count > 2) {

        // Finish handling the last input byte.
        index = in_buffer[2] & 0x3F;
        out_buffer[3] = code_table[index];
      }
    }

    // Write the encoded data.
    result.append(reinterpret_cast<char *>(out_buffer), 4);

    // Reset the buffers to their default state.
    memset(in_buffer, 0, 3);
    memset(out_buffer, '=', 4);
  }
  std::fclose(infile);
  return result;
}

// Helper function for read_string (below).
static bool isBase64(int Ch)
{
  if (std::isalpha(Ch) || std::isdigit(Ch) ||
      Ch == '+' || Ch == '/' || Ch == '=') return true;
  return false;
}

// Helper function for FileSystem::decode (below).
static int read_string(
  unsigned char *buffer, std::string::size_type desired, std::string &contents)
{
  std::string::size_type count = 0;
  std::string::size_type index = 0;

  while (count < desired && index < contents.size()) {
    if (isBase64(contents[index])) {
      *buffer++ = contents[index];
      ++count;
    }
    ++index;
  }
  contents.erase(contents.begin(), contents.begin() + index);
  return count;
}

void FileSystem::decode(const std::string &name, const std::string &contents)
{
  // Creating a temp here is probably not great. What if the file is
  // large? However, read_string modifies the string as it reads it (to
  // avoid having to keep track of any internal state) so I need to copy
  // the constant string given as an argument.
  //
  std::string temp(contents);

  std::string path(normalize_path(name));
  std::FILE *outfile = std::fopen(path.c_str(), "wb");
  if (outfile == NULL) return;

  unsigned char in_buffer[4];
  unsigned char out_buffer[3];
  std::string::size_type count;

  while ((count = read_string(in_buffer, 4, temp)) != 0) {
    // Input base64 is corrupt.
    if (count != 4 || in_buffer[0] == '=' || in_buffer[1] == '=') break;

    const char *p;
    int   value;

    p = std::find(code_table, code_table + sizeof(code_table), in_buffer[0]);
    value = p - code_table;
    out_buffer[0] = value << 2;

    p = std::find(code_table, code_table + sizeof(code_table), in_buffer[1]);
    value = p - code_table;
    out_buffer[0] |= (value & 0x30) >> 4;
    out_buffer[1]  = (value & 0x0F) << 4;
    count = 1;

    if (in_buffer[2] != '=') {

      p = std::find(code_table, code_table + sizeof(code_table), in_buffer[2]);
      value = p - code_table;
      out_buffer[1] |= (value & 0x3C) >> 2;
      out_buffer[2]  = (value & 0x03) << 6;
      count = 2;

      if (in_buffer[3] != '=') {

        p = std::find(code_table, code_table + sizeof(code_table), in_buffer[3]);
        value = p - code_table;
        out_buffer[2] |= value;
        count = 3;
      }
    }

    std::fwrite(out_buffer, sizeof(unsigned char), count, outfile);
  }

  std::fclose(outfile);
}


std::string FileSystem::getRoot()
{
  return system_root;
}


void FileSystem::lock()
{
  struct sembuf op;

  std::memset(&op, 0, sizeof(op));  // Make sure all fields zero/NULL.
  op.sem_num =  0;
  op.sem_op  = -1;
  semop(semid, &op, 1);
}


void FileSystem::unlock()
{
  struct sembuf op;

  std::memset(&op, 0, sizeof(op));  // Make sure all fields zero/NULL.
  op.sem_num = 0;
  op.sem_op  = 1;
  semop(semid, &op, 1);
}
