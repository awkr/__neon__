#pragma once

#include "renderer/types.h"
#include <memory>

struct CommandPool;
struct ImageView;

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

  void imageMemoryBarrier(const ImageView &imageView,
                          const ImageMemoryBarrier &memoryBarrier) const;
  void setViewport(const VkViewport &viewport) const;
  void setScissor(const VkRect2D &scissor) const;

  VkCommandBufferLevel level{};
  VkCommandBuffer handle{VK_NULL_HANDLE};
  CommandBufferState state{CommandBufferState::Created};
};
