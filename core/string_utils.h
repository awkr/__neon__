#pragma once

#include <cstring>

inline bool stringEquals(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}
