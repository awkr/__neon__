#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct FencePool {
  FencePool(Device &device) : device{device} {}
  ~FencePool();
  bool requestFence(VkFence &fence);
  bool wait(uint64_t timeout = UINT64_MAX);
  bool reset();

  Device &device;
  std::vector<VkFence> fences;
  uint32_t activeFenceCount{0};
};
