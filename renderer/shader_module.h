#pragma once

#include "renderer/resource.h"
#include <string>
#include <vulkan/vulkan.h>

struct ShaderSource {
  uint64_t id;
  std::string filepath;
  std::string source;
};

bool createShaderSource(ShaderSource *shaderSource,
                        const std::string &filepath);

struct ShaderVariant {
public:
  void addDefinitions(
      const std::vector<std::pair<std::string, std::string>> &definitions);
  void addDefinition(const std::pair<std::string, std::string> &definition);

  uint64_t getId() const { return id; }

private:
  void updateId();

  uint64_t id{0};
  std::string preamble;
  std::vector<std::string> processes;
};

struct ShaderModule : public Resource {
public:
  ShaderModule(VkShaderStageFlagBits stage, const ShaderSource &source,
               const std::string &entry, const ShaderVariant &variant);

  uint64_t getId() const { return id; }

  bool prepare() override;

private:
  uint64_t id{0};
};
