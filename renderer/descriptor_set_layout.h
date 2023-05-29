#pragma once

#include "renderer/resource.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

struct Device;
struct ShaderModule;
struct ShaderResource;

struct DescriptorSetLayout : public Resource {
  static std::unique_ptr<DescriptorSetLayout>
  make(Device &resource, uint32_t setIndex,
       const std::vector<ShaderModule *> &shaderModules,
       const std::vector<ShaderResource> &shaderResources);

  explicit DescriptorSetLayout(Device &device) : device{device} {}

  ~DescriptorSetLayout() override;

  VkDescriptorSetLayout getHandle() const { return handle; }

  uint32_t getIndex() const { return setIndex; }

  const VkDescriptorSetLayoutBinding *
  getLayoutBinding(uint32_t bindingIndex) const;

  VkDescriptorBindingFlagsEXT getLayoutBindingFlag(uint32_t bindingIndex) const;

  const std::vector<VkDescriptorBindingFlagsEXT> &getBindingFlags() const {
    return bindingFlags;
  }

private:
  Device                                  &device;
  VkDescriptorSetLayout                    handle{VK_NULL_HANDLE};
  uint32_t                                 setIndex{0};
  std::vector<VkDescriptorBindingFlagsEXT> bindingFlags;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindingsLookup;
  std::unordered_map<uint32_t, VkDescriptorBindingFlagsEXT>  bindingFlagsLookup;
};
