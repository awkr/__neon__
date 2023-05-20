#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Queue {
  bool submit(const std::vector<VkSubmitInfo> &submitInfos,
              VkFence fence) const;
  void present(const VkPresentInfoKHR &presentInfo) const;

  VkQueue handle{VK_NULL_HANDLE};
  uint32_t familyIndex{UINT32_MAX};
  VkQueueFamilyProperties properties{};
  VkBool32 supportPresent{false};
};
