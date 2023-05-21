#include "renderer/render_frame.h"
#include "renderer/device.h"

RenderFrame::RenderFrame(Device &device,
                         std::unique_ptr<RenderTarget> &&renderTarget,
                         size_t threadCount)
    : device{device}, semaphorePool{device}, fencePool{device},
      renderTarget{std::move(renderTarget)}, threadCount{threadCount} {}

RenderFrame::~RenderFrame() { renderTarget.reset(); }

bool RenderFrame::requestOutSemaphore(VkSemaphore &semaphore) {
  return semaphorePool.requestOutSemaphore(semaphore);
}

bool RenderFrame::requestSemaphore(VkSemaphore &semaphore) {
  return semaphorePool.requestSemaphore(semaphore);
}

void RenderFrame::releaseSemaphore(VkSemaphore semaphore) {
  semaphorePool.releaseSemaphore(semaphore);
}

bool RenderFrame::requestCommandBuffer(CommandBuffer **commandBuffer,
                                       const Queue *queue,
                                       CommandBufferResetMode resetMode,
                                       VkCommandBufferLevel level,
                                       size_t threadIndex) {
  std::vector<std::unique_ptr<CommandPool>> *commandPool{nullptr};
  bool ok = getCommandPool(queue, resetMode, &commandPool);
  if (!ok) { return false; }
  auto it =
      std::find_if(commandPool->begin(), commandPool->end(),
                   [&threadIndex](std::unique_ptr<CommandPool> &commandPool) {
                     return commandPool->threadIndex == threadIndex;
                   });
  return (*it)->requestCommandBuffer(commandBuffer, level);
}

bool RenderFrame::requestFence(VkFence &fence) {
  return fencePool.requestFence(fence);
}

void RenderFrame::reset() {
  fencePool.wait();
  fencePool.reset();
  for (auto &queueCommandPools : commandPools) {
    for (auto &commandPool : queueCommandPools.second) {
      commandPool->resetPool();
    }
  }
  semaphorePool.reset();
}

bool RenderFrame::getCommandPool(
    const Queue *queue, CommandBufferResetMode resetMode,
    std::vector<std::unique_ptr<CommandPool>> **commandPool) {
  if (auto it = commandPools.find(queue->familyIndex);
      it != commandPools.end()) {
    if (it->second.front()->resetMode != resetMode) {
      device.waitIdle();
      commandPools.erase(it); // Delete pools
    } else {
      *commandPool = &(it->second);
      return true;
    }
  }
  std::vector<std::unique_ptr<CommandPool>> queueCommandPools;
  for (size_t i = 0; i < threadCount; ++i) {
    auto pool = CommandPool::make(device, queue->familyIndex, resetMode);
    if (!pool) { return false; }
    queueCommandPools.emplace_back(std::move(pool));
  }

  auto it =
      commandPools.emplace(queue->familyIndex, std::move(queueCommandPools));
  if (!it.second) { return false; }
  *commandPool = &(it.first->second);
  return true;
}

void RenderFrame::updateRenderTarget(
    std::unique_ptr<RenderTarget> &&renderTarget) {
  this->renderTarget = std::move(renderTarget);
}
