#include "renderer/semaphore_pool.h"
#include "renderer/device.h"

SemaphorePool::~SemaphorePool() {
  reset();
  for (auto semaphore : semaphores) {
    vkDestroySemaphore(device.handle, semaphore, nullptr);
  }
  semaphores.clear();
}

bool SemaphorePool::requestOutSemaphore(VkSemaphore &semaphore) {
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

void SemaphorePool::reset() {
  activeSemaphoreCount = 0;
  // Recycle the released semaphores
  for (auto &semaphore : releasedSemaphores) {
    semaphores.emplace_back(semaphore);
  }
  releasedSemaphores.clear();
}
