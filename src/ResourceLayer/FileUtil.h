#pragma once
#include <iostream>
#include <string>
class FileUtil {
public:
  static std::string workingDirectory;

  static void setWorkingDirectory(const std::string &dir) {
    workingDirectory = dir;
  }

  static std::string getFullPath(const std::string &relativePath) {
    return workingDirectory + "/" + relativePath;
  }
}; // namespace FileUtil
