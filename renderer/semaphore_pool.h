#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct SemaphorePool {
  SemaphorePool(Device &device) : device{device} {}
  bool requestOwnedSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);

private:
  Device &device;
  std::vector<VkSemaphore> semaphores;
  std::vector<VkSemaphore> releasedSemaphores;
  uint32_t activeSemaphoreCount{0};
};
