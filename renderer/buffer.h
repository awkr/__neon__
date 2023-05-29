#pragma once

#include <vulkan/vulkan.h>

struct Device;

struct Buffer {
  explicit Buffer(Device &device) : device{device} {}

  ~Buffer();

  Device &device;

  VkBuffer handle{VK_NULL_HANDLE};
};
