/*****************************************************************************
FILE          : Logger.cpp
LAST REVISION : 2004-11-02

Do we want to define a standard header for our source files?

*****************************************************************************/

#include <ctime>
#include <iomanip>
#include <sstream>
#include "Logger.h"


Logger::Logger(FileSystem *f, const std::string &name) : fs(f)
{
  logfile_handle = fs->open(name.c_str(), FileSystem::Write);
}


Logger::~Logger()
{
  if (logfile_handle != -1) fs->close(logfile_handle);
}


void Logger::write(const std::string &message)
{
  std::time_t raw_time = std::time(0);
  std::tm *cooked_time = std::localtime(&raw_time);

  std::stringstream formatter;

  formatter << std::setfill('0');
  formatter << std::setw(4) << cooked_time->tm_year + 1900 << "-"
            << std::setw(2) << cooked_time->tm_mon  << "-"
            << std::setw(2) << cooked_time->tm_mday << "T"
            << std::setw(2) << cooked_time->tm_hour << ":"
            << std::setw(2) << cooked_time->tm_min  << ":"
            << std::setw(2) << cooked_time->tm_sec  << " "
            << message;

  fs->writeLine(logfile_handle, formatter.str());
  fs->flush(logfile_handle);
}

