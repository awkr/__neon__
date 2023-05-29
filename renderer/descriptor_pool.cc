#include "renderer/descriptor_pool.h"
#include "renderer/device.h"

std::unique_ptr<DescriptorPool>
DescriptorPool::make(Device                    &device,
                     const DescriptorSetLayout &descriptorSetLayout) {
  auto descriptorPool = std::make_unique<DescriptorPool>(device);
  descriptorPool->descriptorSetLayout = &descriptorSetLayout;
  return descriptorPool;
}

DescriptorPool::~DescriptorPool() {
  for (auto pool : pools) {
    vkDestroyDescriptorPool(device.handle, pool, nullptr);
  }
}

VkDescriptorSet DescriptorPool::allocate() {
  poolIndex = findAvailablePool(poolIndex);

  ++poolSetsCount[poolIndex];

  auto setLayout = descriptorSetLayout->getHandle();

  VkDescriptorSetAllocateInfo allocateInfo{
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  allocateInfo.descriptorPool     = pools[poolIndex];
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts        = &setLayout;

  VkDescriptorSet handle{VK_NULL_HANDLE};
  auto result = vkAllocateDescriptorSets(device.handle, &allocateInfo, &handle);
  if (result != VK_SUCCESS) {
    --poolSetsCount[poolIndex];
    return VK_NULL_HANDLE;
  }

  setPoolMapping.emplace(handle, poolIndex);

  return handle;
}

uint32_t DescriptorPool::findAvailablePool(uint32_t index) {
  if (pools.size() <= index) { // Create a new pool
    VkDescriptorPoolCreateInfo createInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};

    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes    = poolSizes.data();
    createInfo.maxSets       = poolMaxSets;

    const auto &bindingFlags = descriptorSetLayout->getBindingFlags();
    for (auto bindingFlag : bindingFlags) {
      if (bindingFlag & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) {
        createInfo.flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
      }
    }

    VkDescriptorPool handle{VK_NULL_HANDLE};
    if (vkCreateDescriptorPool(device.handle, &createInfo, nullptr, &handle) !=
        VK_SUCCESS) {
      return 0;
    }

    pools.push_back(handle);
    poolSetsCount.push_back(0);

    return index;
  } else if (poolSetsCount[index] < poolMaxSets) {
    return index;
  }

  return findAvailablePool(++index);
}
