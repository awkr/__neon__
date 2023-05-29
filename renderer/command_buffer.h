#pragma once

#include "renderer/descriptor_set_layout.h"
#include "renderer/pipeline_state.h"
#include "renderer/resource_binding_state.h"
#include "renderer/types.h"
#include <memory>

struct Device;
struct CommandPool;
struct ImageView;
struct RenderTarget;
struct Subpass;
struct RenderPass;
struct Framebuffer;
struct Buffer;

enum class CommandBufferResetMode {
  ResetPool,
  ResetIndividually,
  AlwaysAllocate,
};

struct RenderPassBinding {
  const RenderPass  *renderPass{nullptr};
  const Framebuffer *framebuffer{nullptr};
};

struct CommandBuffer {
  static std::unique_ptr<CommandBuffer> make(CommandPool         &commandPool,
                                             VkCommandBufferLevel level);

  explicit CommandBuffer(Device &device) : device{device} {}

  void reset(CommandBufferResetMode resetMode);

  bool begin(VkCommandBufferUsageFlags usage) const;
  bool end() const;

  void imageMemoryBarrier(const ImageView          &imageView,
                          const ImageMemoryBarrier &memoryBarrier) const;
  void setViewport(const VkViewport &viewport) const;
  void setScissor(const VkRect2D &scissor) const;

  bool beginRenderPass(const RenderTarget              &renderTarget,
                       const std::vector<LoadStoreOp>  &loadStoreOps,
                       const std::vector<VkClearValue> &clearValues,
                       const std::vector<std::unique_ptr<Subpass>> &subpasses);
  void endRenderPass() const;
  void nextSubpass();

  // Set pipeline states
  void setRasterizationState(const RasterizationState &state);
  void setMultisampleState(const MultisampleState &state);
  void bindPipelineLayout(const PipelineLayout &pipelineLayout);

  void setVertexInputState(const VertexInputState &state);
  void bindVertexBuffers(
      uint32_t                                                 firstBinding,
      const std::vector<std::reference_wrapper<const Buffer>> &buffers,
      const std::vector<VkDeviceSize>                         &offsets) const;
  void bindIndexBuffer();

  /**
   * Flushes the command buffer, pushing the changes
   * @param pipelineBindPoint The type of pipeline to be flushed
   */
  void flush(VkPipelineBindPoint pipelineBindPoint);

  void drawIndexed();
  void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
            uint32_t firstInstance);

  void beginRenderPass(const RenderPass                &renderPass,
                       const RenderTarget              &renderTarget,
                       const Framebuffer               &framebuffer,
                       const std::vector<VkClearValue> &clearValues);
  void flushPipelineState(VkPipelineBindPoint pipelineBindPoint);
  void flushPushConstants();
  void flushDescriptorState(VkPipelineBindPoint pipelineBindPoint);

  Device              &device;
  VkCommandBufferLevel level{};
  CommandPool         *commandPool{nullptr};
  VkCommandBuffer      handle{VK_NULL_HANDLE};

  RenderPassBinding renderPassBinding{};
  PipelineState     pipelineState{};

  // Descriptor set state
  ResourceBindingState resourceBindingState{};

  std::vector<uint8_t> storedPushConstants;

  bool updateAfterBind{false};
};
