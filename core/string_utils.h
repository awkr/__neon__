#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

inline bool equals(const char *a, const char *b) { return strcmp(a, b) == 0; }

inline std::vector<std::string> split(const std::string &input, char delim) {
  std::vector<std::string> tokens;

  std::stringstream sstream(input);
  std::string       token{};
  while (std::getline(sstream, token, delim)) { tokens.emplace_back(token); }

  return tokens;
}

inline std::string &trim(std::string &s) {
  const char *whitespaces = " \t\n\r\f\v";
  s.erase(s.find_last_not_of(whitespaces) + 1);
  s.erase(0, s.find_first_not_of(whitespaces));
  return s;
}
