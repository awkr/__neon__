#pragma once

#include "renderer/resource.h"
#include "renderer/shader_module.h"
#include <memory>
#include <unordered_map>

struct Device;
struct DescriptorSetLayout;

struct PipelineLayout : public Resource {
  static std::unique_ptr<PipelineLayout>
  make(Device &device, const std::vector<ShaderModule *> &shaderModules);

  explicit PipelineLayout(Device &device) : device{device} {}
  ~PipelineLayout() override;

  bool hasDescriptorSetLayout(uint32_t setIndex) const;

  std::vector<const ShaderResource *>
  getShaderResources(const ShaderResourceType &type,
                     VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL) const;

  VkShaderStageFlags getPushConstantRangeStage(uint32_t size,
                                               uint32_t offset = 0) const;

  const std::unordered_map<uint32_t, std::vector<ShaderResource>> &
  getShaderSets() const {
    return shaderSets;
  };

  const DescriptorSetLayout *getDescriptorSetLayout(uint32_t setIndex) const;

  const std::vector<ShaderModule *> &getShaderModules() const {
    return shaderModules;
  }

  VkPipelineLayout getHandle() const { return handle; }

  Device          &device;
  VkPipelineLayout handle{VK_NULL_HANDLE};
  // The shader modules that this pipeline layout uses
  std::vector<ShaderModule *>                               shaderModules;
  std::unordered_map<std::string, ShaderResource>           shaderResources;
  std::unordered_map<uint32_t, std::vector<ShaderResource>> shaderSets;
  // one layout per set
  std::vector<DescriptorSetLayout *> descriptorSetLayouts;
};
