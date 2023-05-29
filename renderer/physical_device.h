#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct PhysicalDevice {
  PhysicalDevice(VkPhysicalDevice physicalDevice);

  PhysicalDevice() = default;

  VkPhysicalDevice handle{VK_NULL_HANDLE};

  VkPhysicalDeviceFeatures features{};

  VkPhysicalDeviceProperties properties{};

  VkPhysicalDeviceMemoryProperties memoryProperties{};

  std::vector<VkQueueFamilyProperties> queueFamilyProperties;
};
