#pragma once

#include "renderer/command_pool.h"
#include "renderer/fence_pool.h"
#include "renderer/render_target.h"
#include "renderer/semaphore_pool.h"

struct CommandBuffer;
struct Queue;

struct RenderFrame {
  RenderFrame(Device &device, RenderTarget &&renderTarget,
              size_t threadCount = 1);

  ~RenderFrame();

  bool requestOutSemaphore(VkSemaphore &semaphore);
  bool requestSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);

  bool requestCommandBuffer(
      CommandBuffer **commandBuffer, const Queue *queue,
      CommandBufferResetMode resetMode = CommandBufferResetMode::ResetPool,
      VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      size_t threadIndex = 0);

  bool requestFence(VkFence &fence);
  void reset();

  RenderTarget *getRenderTarget() { return &renderTarget; }

private:
  bool getCommandPool(const Queue *queue, CommandBufferResetMode resetMode,
                      std::vector<std::unique_ptr<CommandPool>> **commandPool);

  Device &device;
  RenderTarget renderTarget{};
  size_t threadCount{1};
  SemaphorePool semaphorePool;
  FencePool fencePool;
  std::unordered_map<uint32_t, std::vector<std::unique_ptr<CommandPool>>>
      commandPools; // Key is queue family index
};
