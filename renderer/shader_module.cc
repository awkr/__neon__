#include "renderer/shader_module.h"
#include "core/file_system.h"

bool createShaderSource(ShaderSource *shaderSource,
                        const std::string &filepath) {
  std::string source{};
  if (!readTextFile(filepath, source)) {
    return false;
  }

  std::hash<std::string> hasher{};
  shaderSource->id = hasher(std::string{source.cbegin(), source.cend()});

  shaderSource->filepath = filepath;
  shaderSource->source = std::move(source);

  return true;
}
