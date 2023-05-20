#include "renderer/fence_pool.h"
#include "renderer/device.h"

FencePool::~FencePool() {
  wait();
  reset();
  for (auto fence : fences) {
    vkDestroyFence(device.handle, fence, nullptr);
  }
  fences.clear();
}

bool FencePool::requestFence(VkFence &fence) {
  if (activeFenceCount < fences.size()) {
    fence = fences[activeFenceCount++];
    return true;
  }
  VkFence handle{VK_NULL_HANDLE};
  VkFenceCreateInfo createInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  if (vkCreateFence(device.handle, &createInfo, nullptr, &handle) !=
      VK_SUCCESS) {
    return false;
  }
  fences.emplace_back(handle);
  ++activeFenceCount;
  fence = fences.back();
  return true;
}

bool FencePool::wait(uint64_t timeout) {
  if (activeFenceCount < 1) { return true; }
  return vkWaitForFences(device.handle, activeFenceCount, fences.data(), true,
                         timeout) == VK_SUCCESS;
}

bool FencePool::reset() {
  if (activeFenceCount < 1) { return true; }
  if (vkResetFences(device.handle, activeFenceCount, fences.data()) !=
      VK_SUCCESS) {
    return false;
  }
  activeFenceCount = 0;
  return true;
}
