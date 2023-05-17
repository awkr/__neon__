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
