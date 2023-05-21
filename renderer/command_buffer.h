#pragma once

#include "renderer/types.h"
#include <memory>

struct Device;
struct CommandPool;
struct ImageView;
struct RenderTarget;
struct Subpass;

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

  CommandBuffer(Device &device) : device{device} {}

  void reset(CommandBufferResetMode resetMode);

  bool begin(VkCommandBufferUsageFlags usage);
  bool end();

  void imageMemoryBarrier(const ImageView &imageView,
                          const ImageMemoryBarrier &memoryBarrier) const;
  void setViewport(const VkViewport &viewport) const;
  void setScissor(const VkRect2D &scissor) const;
  bool beginRenderPass(const RenderTarget &renderTarget,
                       const std::vector<LoadStoreOp> &loadStoreOps,
                       const std::vector<VkClearValue> &clearValues,
                       const std::vector<std::unique_ptr<Subpass>> &subpasses);
  void endRenderPass();
  void nextSubpass();

  Device &device;
  VkCommandBufferLevel level{};
  VkCommandBuffer handle{VK_NULL_HANDLE};
  CommandBufferState state{CommandBufferState::Created};
};
