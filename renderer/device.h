#pragma once

#include "renderer/queue.h"
#include <vector>

struct Device {
  bool getQueue(VkQueueFlags requiredFlags, uint32_t index, Queue &queue) const;
  VkResult waitIdle() const;

  VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
  VkDevice handle{VK_NULL_HANDLE};
  std::vector<std::vector<Queue>> queues;
};

bool createDevice(VkInstance instance, Device *device);

void destroyDevice(Device *device);

VkFormat chooseDepthFormat(VkPhysicalDevice physicalDevice,
                           bool depthOnly = false,
                           const std::vector<VkFormat> &depthFormatPriority = {
                               VK_FORMAT_D32_SFLOAT,
                               VK_FORMAT_D24_UNORM_S8_UINT,
                               VK_FORMAT_D16_UNORM,
                           });

bool getMemoryTypeIndex(Device *device, uint32_t typeBits,
                        VkMemoryPropertyFlags property, uint32_t &typeIndex);

bool isDepthOnlyFormat(VkFormat format);
bool isDepthStencilFormat(VkFormat format);
