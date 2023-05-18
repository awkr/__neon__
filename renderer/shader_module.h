#pragma once

#include <string>

struct ShaderSource {
  uint64_t id;
  std::string filepath;
  std::string source;
};

bool createShaderSource(ShaderSource *shaderSource,
                        const std::string &filepath);
