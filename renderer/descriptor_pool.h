#pragma once

#include "renderer/resource.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

struct Device;
struct DescriptorSetLayout;

struct DescriptorPool : public Resource {
  static std::unique_ptr<DescriptorPool>
  make(Device &device, const DescriptorSetLayout &descriptorSetLayout);

  explicit DescriptorPool(Device &device) : device{device} {}

  ~DescriptorPool() override;

  VkDescriptorSet allocate();

  /**
   *
   * @param index Pool index
   * @return
   */
  uint32_t findAvailablePool(uint32_t index);

  Device                           &device;
  const DescriptorSetLayout        *descriptorSetLayout{nullptr};
  uint32_t                          poolMaxSets{0};
  std::vector<VkDescriptorPoolSize> poolSizes;
  std::vector<VkDescriptorPool>     pools;
  std::vector<uint32_t>             poolSetsCount;
  uint32_t                          poolIndex{0};

  // Map between descriptor set and pool index
  std::unordered_map<VkDescriptorSet, uint32_t> setPoolMapping;
};
