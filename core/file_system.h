#pragma once

#include <fstream>
#include <sstream>

bool readTextFile(const std::string &filepath, std::string &source) {
  std::ifstream fs(filepath);
  if (!fs.is_open()) {
    return false;
  }

  std::stringstream ss;
  ss << fs.rdbuf();
  source = ss.str();

  return true;
}
