#pragma once

#include "renderer/resource.h"
#include "renderer/resource_binding_state.h"

struct Device;
struct DescriptorSetLayout;
struct DescriptorPool;

struct DescriptorSet : public Resource {
  static std::unique_ptr<DescriptorSet>
  make(Device &device, const DescriptorSetLayout &descriptorSetLayout,
       const DescriptorPool                     &descriptorPool,
       const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
       const BindingMap<VkDescriptorImageInfo>  &imageInfos);

  DescriptorSet(Device &device, const DescriptorSetLayout &descriptorSetLayout,
                DescriptorPool                           &descriptorPool,
                const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
                const BindingMap<VkDescriptorImageInfo>  &imageInfos);

  // The descriptor set handle is managed by the pool, and will be destroyed
  // when the pool is reset.
  ~DescriptorSet() override = default;

  bool prepare();

  void applyWrites() const;

  void update(const std::vector<uint32_t> &bindingsToUpdate);

  Device &device;

  const DescriptorSetLayout *descriptorSetLayout;

  BindingMap<VkDescriptorBufferInfo> bufferInfos;

  BindingMap<VkDescriptorImageInfo> imageInfos;

  VkDescriptorSet handle{VK_NULL_HANDLE};

  // the write operations for the descriptor set
  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  std::unordered_map<uint32_t, size_t> updatedBindings;
};
