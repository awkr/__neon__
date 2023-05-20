#pragma once

#include <vulkan/vulkan.h>

struct Queue {
  VkQueue handle{VK_NULL_HANDLE};
  uint32_t familyIndex{UINT32_MAX};
  VkQueueFamilyProperties properties{};
};
