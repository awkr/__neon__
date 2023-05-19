#include "renderer/semaphore_pool.h"
#include "renderer/device.h"

bool SemaphorePool::requestOwnedSemaphore(VkSemaphore &semaphore) {
  if (activeSemaphoreCount < semaphores.size()) {
    semaphore = semaphores.back();
    semaphores.pop_back();
    return true;
  }
  // Create one, and don't keep track of it
  VkSemaphoreCreateInfo semaphoreCreateInfo{
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  if (vkCreateSemaphore(device.handle, &semaphoreCreateInfo, nullptr,
                        &semaphore) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void SemaphorePool::releaseSemaphore(VkSemaphore semaphore) {
  releasedSemaphores.emplace_back(semaphore);
}
