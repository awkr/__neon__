#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Device {
  VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
  VkDevice handle{VK_NULL_HANDLE};
  std::vector<std::vector<VkQueue>> queues;
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
