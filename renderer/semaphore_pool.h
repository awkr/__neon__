#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct SemaphorePool {
  SemaphorePool(Device &device) : device{device} {}
  ~SemaphorePool();
  bool requestOutSemaphore(VkSemaphore &semaphore);
  bool requestSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);
  void reset();

private:
  Device &device;
  std::vector<VkSemaphore> semaphores;
  std::vector<VkSemaphore> releasedSemaphores;
  uint32_t activeSemaphoreCount{0};
};
