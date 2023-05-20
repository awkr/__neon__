#pragma once

#include "renderer/command_pool.h"
#include "renderer/render_target.h"
#include "renderer/semaphore_pool.h"

struct CommandBuffer;
struct Queue;

struct RenderFrame {
  RenderFrame(Device &device, RenderTarget &&renderTarget,
              size_t threadCount = 1);

  ~RenderFrame();

  bool requestOutSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);

  bool requestCommandBuffer(
      CommandBuffer **commandBuffer, Queue &queue,
      CommandBufferResetMode resetMode = CommandBufferResetMode::ResetPool,
      VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      size_t threadIndex = 0);

  void reset();

private:
  bool getCommandPool(const Queue &queue, CommandBufferResetMode resetMode,
                      std::vector<std::unique_ptr<CommandPool>> **commandPool);

  Device &device;
  RenderTarget renderTarget{};
  size_t threadCount{1};
  SemaphorePool semaphorePool;
  std::unordered_map<uint32_t, std::vector<std::unique_ptr<CommandPool>>>
      commandPools; // Key is queue family index
};
