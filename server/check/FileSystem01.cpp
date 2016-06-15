/*****************************************************************************
FILE          : FileSystem01.cpp
LAST REVISION : 2004-11-20
SUMMARY       : Test program that exercises FileSystem code.

*****************************************************************************/

#include <iostream>
#include <string>
#include "FileSystem.h"

bool read_test01(FileSystem &fs)
{
  int handle = fs.open("file1.txt", FileSystem::Read);
  if (handle == -1) {
    std::cout << "FileSystem: Error opening existing file for reading\n";
    return false;
  }

  std::string line;
  if (!fs.readLine(handle, line) || line != "") {
    std::cout << "FileSystem: Error reading first (empty) line of a file\n";
    fs.close(handle);
    return false;
  }
  if (!fs.readLine(handle, line) || line != "Line 2") {
    std::cout << "FileSystem: Error reading second line of a file\n";
    fs.close(handle);
    return false;
  }
  if (fs.readLine(handle, line)) {
    std::cout << "FileSystem: Reading past end of file is allowed\n";
    fs.close(handle);
    return false;
  }

  fs.close(handle);
  return true;
}


bool read_test02(FileSystem &fs)
{
  int count = 0;

  // Classic loop to read a file a line at a time.
  int handle = fs.open("file1.txt", FileSystem::Read);
  if (handle == -1) {
    std::cout << "FileSystem: Problem reopening a file\n";
    return false;
  }

  std::string line;
  while (fs.readLine(handle, line)) {
    ++count;
  }
  fs.close(handle);

  if (count != 2) {
    std::cout << "FileSystem: Line reading loop miscounts lines\n";
    return false;
  }
  return true;
}


bool read_test03(FileSystem &fs)
{
  int handle = fs.open("file2.txt", FileSystem::Read);
  if (handle == -1) {
    std::cout << "FileSystem: Unable to open an empty file\n";
    return false;
  }

  std::string line;
  if (fs.readLine(handle, line)) {
    std::cout << "FileSystem: Able to read from an empty file\n";
    fs.close(handle);
    return false;
  }
  return true;
}


bool read_test04(FileSystem &fs)
{
  int handle = fs.open("non-existent.txt", FileSystem::Read);
  if (handle != -1) {
    std::cout << "FileSystem: Able to open a nonexistent file for reading\n";
    return false;
  }
  return true;
}


bool write_test01(FileSystem &fs)
{
  int handle = fs.open("file3.txt", FileSystem::Write);
  if (handle == -1) {
    std::cout << "FileSystem: Unable to open a file for writing\n";
    return false;
  }

  if (!fs.writeLine(handle, "Hello, World!")) {
    std::cout << "FileSystem: Unable to write a line into an output file\n";
    fs.close(handle);
    return false;
  }
  fs.close(handle);

  handle = fs.open("file3.txt", FileSystem::Read);
  if (handle == -1) {
    std::cout << "FileSystem: Unable to re-open output file for reading\n";
    return false;
  }

  std::string line;
  if (!fs.readLine(handle, line) || line != "Hello, World!") {
    std::cout << "FileSystem: Unable to read back previous output\n";
    fs.close(handle);
    return false;
  }

  fs.close(handle);
  return true;
}


bool append_test01(FileSystem &fs)
{
  int handle;

  if ((handle = fs.open("xyzzy.txt", FileSystem::Write)) == -1) {
    std::cout << "FileSystem: Unable to open output file\n";
    return false;
  }
  if (!fs.writeLine(handle, "Hello, World!")) {
    std::cout << "FileSystem: Unable to write a line into an output file\n";
    fs.close(handle);
    return false;
  }
  fs.close(handle);

  if ((handle = fs.open("xyzzy.txt", FileSystem::Append)) == -1) {
    std::cout << "FileSystem: Unable to open output file for append\n";
    return false;
  }
  if (!fs.writeLine(handle, "Goodbye, World!")) {
    std::cout << "FileSystem: Unable to append a line onto an output file\n";
    fs.close(handle);
    return false;
  }
  fs.close(handle);

  if ((handle = fs.open("xyzzy.txt", FileSystem::Read)) == -1) {
    std::cout << "FileSystem: Unable to open input file\n";
    return false;
  }
  std::string line1, line2;
  if (!fs.readLine(handle, line1) || !fs.readLine(handle, line2)) {
    std::cout << "FileSystem: Unable to read back appended lines\n";
    fs.close(handle);
    return false;
  }
  if (line1 != "Hello, World!" || line2 != "Goodbye, World!") {
    std::cout << "FileSystem: Appended data incorrect\n";
    fs.close(handle);
    return false;
  }

  fs.remove("xyzzy.txt");
  return true;
}


bool remove_test01(FileSystem &fs)
{
  if (!fs.remove("file3.txt")) {
    std::cout << "FileSystem: Unable to remove a file\n";
    return false;
  }

  int handle = fs.open("file3.txt", FileSystem::Read);
  if (handle != -1) {
    std::cout << "FileSystem: Able to open previously removed file\n";
    return false;
  }
  return true;
}


bool filelist_test01(FileSystem &fs)
{
  FileSystem::namelist_type expected_list;

  expected_list.push_back("file1.txt");
  expected_list.push_back("file2.txt");

  FileSystem::namelist_type list = fs.getFileList("");
  if (list != expected_list) {
    std::cout << "FileSystem: Unable to list files in root directory\n";
    return false;
  }
  return true;
}


bool subdir_test01(FileSystem &fs)
{
  if (!fs.rmdir("subdir1")) {
    std::cout << "FileSystem: Unable to remove a directory\n";
    return false;
  }

  if (!fs.mkdir("subdir2")) {
    std::cout << "FileSystem: Unable to create a directory\n";
    return false;
  }

  int handle = fs.open("subdir2/file.txt", FileSystem::Write);
  if (handle == -1) {
    std::cout << "FileSystem: Unable to create a file in a directory\n";
    return false;
  }
  fs.close(handle);
  return true;
}


int main()
{
  FileSystem fs("./fs");

  if (!read_test01(fs)    ) return 1;
  if (!read_test02(fs)    ) return 1;
  if (!read_test03(fs)    ) return 1;
  if (!read_test04(fs)    ) return 1;
  if (!write_test01(fs)   ) return 1;  // Leaves a file3.txt
  if (!remove_test01(fs)  ) return 1;  // Removes file3.txt
  if (!append_test01(fs)  ) return 1;
  if (!filelist_test01(fs)) return 1;  // Expects no file3.txt
  if (!subdir_test01(fs)  ) return 1;
  return 0;
}
