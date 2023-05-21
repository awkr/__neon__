#pragma once

#include <memory>
#include <vulkan/vulkan.h>

struct CommandPool;

enum class CommandBufferResetMode {
  ResetPool,
  ResetIndividually,
  AlwaysAllocate,
};

enum class CommandBufferState {
  Created,
  Recording,
  Executable,
};

struct CommandBuffer {
  static std::unique_ptr<CommandBuffer> make(CommandPool *commandPool,
                                             VkCommandBufferLevel level);

  void reset(CommandBufferResetMode resetMode);

  bool begin(VkCommandBufferUsageFlags usage);
  bool end();

  VkCommandBufferLevel level{};
  VkCommandBuffer handle{VK_NULL_HANDLE};
  CommandBufferState state{CommandBufferState::Created};
};
