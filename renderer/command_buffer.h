#pragma once

#include <memory>
#include <vulkan/vulkan.h>

struct CommandPool;

enum class CommandBufferResetMode {
  ResetPool,
  ResetIndividually,
  AlwaysAllocate,
};

struct CommandBuffer {
  static std::unique_ptr<CommandBuffer> make(CommandPool *commandPool,
                                             VkCommandBufferLevel level);

  VkCommandBuffer handle{VK_NULL_HANDLE};
};
