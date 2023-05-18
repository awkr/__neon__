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

void ShaderVariant::addDefinitions(
    const std::vector<std::pair<std::string, std::string>> &definitions) {
  for (const auto &definition : definitions) {
    addDefinition(definition);
  }
}

void ShaderVariant::addDefinition(
    const std::pair<std::string, std::string> &definition) {
  const auto &identifier = definition.first;
  const auto &token = definition.second;

  preamble.append("#define " + identifier + " " + token + "\n");
  processes.emplace_back("D" + identifier + " " + token);

  updateId();
}

void ShaderVariant::updateId() {
  std::hash<std::string> hasher{};
  id = hasher(preamble);
}

ShaderModule::ShaderModule(VkShaderStageFlagBits stage,
                           const ShaderSource &source, const std::string &entry,
                           const ShaderVariant &variant) {
  // TODO
}

bool ShaderModule::prepare() {
  // TODO
  return false;
}
