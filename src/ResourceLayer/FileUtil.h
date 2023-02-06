#pragma once
#include <string>
namespace FileUtil {
static std::string workingDirectory;

inline void setWorkingDirectory(const char *dir) {
  workingDirectory = std::string(dir);
}

inline std::string getFullPath(const std::string &relativePath) {
  return workingDirectory + "/" + relativePath;
}
} // namespace FileUtil