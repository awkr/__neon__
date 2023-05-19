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
  const std::string &getPreamble() const { return preamble; }
  const std::vector<std::string> &getProcesses() const { return processes; }

private:
  void updateId();

  uint64_t id{0};
  std::string preamble;
  std::vector<std::string> processes;
};

struct ShaderResource {};

struct ShaderModule : public Resource {
public:
  uint64_t getId() const { return id; }

  static std::unique_ptr<ShaderModule> make(VkShaderStageFlagBits stage,
                                            const ShaderSource &source,
                                            const std::string &entry,
                                            const ShaderVariant &variant);

private:
  uint64_t id{0};
  std::vector<uint32_t> spirv;
  std::vector<ShaderResource> resources;
};
