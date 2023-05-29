#include "renderer/physical_device.h"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice)
    : handle{physicalDevice} {
  vkGetPhysicalDeviceFeatures(physicalDevice, &features);
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

  uint32_t queueFamilyPropertiesCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      physicalDevice, &queueFamilyPropertiesCount, nullptr);
  queueFamilyProperties.resize(queueFamilyPropertiesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                                           &queueFamilyPropertiesCount,
                                           queueFamilyProperties.data());
}
