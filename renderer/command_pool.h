#pragma once

#include "renderer/command_buffer.h"
#include <vector>

struct Device;

struct CommandPool {
  static std::unique_ptr<CommandPool>
  make(Device &device, uint32_t queueFamilyIndex,
       CommandBufferResetMode resetMode = CommandBufferResetMode::ResetPool,
       size_t threadIndex = 0);

  CommandPool(Device &device) : device{device} {}
  ~CommandPool();

  bool requestCommandBuffer(
      CommandBuffer **commandBuffer,
      VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

  Device &device;
  CommandBufferResetMode resetMode{CommandBufferResetMode::ResetPool};
  VkCommandPool handle{VK_NULL_HANDLE};
  size_t threadIndex{0};

  std::vector<std::unique_ptr<CommandBuffer>> primaryCommandBuffers;
  uint32_t activePrimaryCommandBufferCount{0};

  std::vector<std::unique_ptr<CommandBuffer>> secondaryCommandBuffers;
  uint32_t activeSecondaryCommandBufferCount{0};
};
