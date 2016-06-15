#!/bin/bash
###########################################################################
# FILE          : run_tests.sh
# LAST REVISION : 2004-11-06
# SUMMARY       : Shell script that exercises test programs.
#
#
###########################################################################

if [ ! -x ./FileSystem01 ]; then
  echo "FileSystem01 has not been built!"
else
  # Create a test file system in the form FileSystem01 expects.
  rm -rf fs
  mkdir fs
  echo "" > fs/file1.txt
  echo "Line 2" >> fs/file1.txt
  touch fs/file2.txt
  mkdir fs/subdir1
  ./FileSystem01
  if [ $? -ne 0 ]; then
    echo "FAIL: FileSystem"
  else
    if [ -d fs/subdir1 ]; then
      echo "FileSystem: fs/subdir1 not removed"
      echo "FAIL: FileSystem"
    elif [ ! -d fs/subdir2 -o ! -f fs/subdir2/file.txt ]; then
      echo "FileSystem: fs/subdir2 not created correctly"
      echo "FAIL: FileSystem"
    else
      rm -rf fs
      echo "PASS: FileSystem"
    fi
  fi
fi
