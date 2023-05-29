#pragma once

#include "renderer/resource.h"
#include <string>
#include <vulkan/vulkan.h>

struct Device;

struct ShaderSource {
  uint64_t    id;
  std::string filepath;
  std::string source;
};

bool createShaderSource(ShaderSource      *shaderSource,
                        const std::string &filepath);

struct ShaderVariant {
public:
  void addDefinitions(
      const std::vector<std::pair<std::string, std::string>> &definitions);
  void addDefinition(const std::pair<std::string, std::string> &definition);

  uint64_t                        getId() const { return id; }
  const std::string              &getPreamble() const { return preamble; }
  const std::vector<std::string> &getProcesses() const { return processes; }

private:
  void updateId();

  uint64_t                 id{0};
  std::string              preamble;
  std::vector<std::string> processes;
};

enum class ShaderResourceType {
  Input,
  InputAttachment,
  Output,
  PushConstant,
  SpecializationConstant,
  Image,
  ImageSampler,
  ImageStorage,
  Sampler,
  BufferUniform,
  BufferStorage,
  Max,
};

enum class ShaderResourceMode {
  Static,
  Dynamic,
  UpdateAfterBind,
};

struct ShaderResource {
  std::string        name;
  VkShaderStageFlags stages;
  ShaderResourceType type;
  ShaderResourceMode mode;
  uint32_t           set;
  uint32_t           binding;
  uint32_t           location;
  uint32_t           arraySize;
  uint32_t           offset;
  uint32_t           size;
};

struct ShaderModule : public Resource {
public:
  static std::unique_ptr<ShaderModule>
  make(Device &device, VkShaderStageFlagBits stage, const ShaderSource &source,
       const std::string &entry, const ShaderVariant &variant);

  uint64_t                           getId() const { return id; }
  const std::vector<ShaderResource> &getResources() const { return resources; }

  VkShaderStageFlagBits getStage() const { return stage; }

  const std::string &getEntryPoint() const { return entryPoint; }

  const std::vector<uint32_t> &getSpirv() const { return spirv; }

  uint64_t                    id{0};
  VkShaderStageFlagBits       stage;
  std::string                 entryPoint;
  std::vector<uint32_t>       spirv;
  std::vector<ShaderResource> resources;
};
