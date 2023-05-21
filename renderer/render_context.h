#pragma once

#include "renderer/command_buffer.h"
#include "renderer/render_frame.h"
#include "renderer/resource_cache.h"
#include "renderer/swapchain.h"

struct RenderContext {
public:
  static std::unique_ptr<RenderContext>
  make(std::unique_ptr<Swapchain> &&swapchain);

  ~RenderContext();

  bool begin(CommandBuffer **commandBuffer);
  bool submit(CommandBuffer *commandBuffer);

  ResourceCache &getResourceCache() { return resourceCache; }

  RenderFrame *getActiveFrame();

private:
  bool beginFrame();
  bool endFrame(VkSemaphore waitSemaphore);

  void waitFrame();

  bool submit(const Queue &graphicsQueue,
              const std::vector<CommandBuffer *> &commandBuffers,
              VkSemaphore waitSemaphore, VkPipelineStageFlags waitPipelineStage,
              VkSemaphore *renderCompleteSemaphore);

  Device *device;
  std::unique_ptr<Swapchain> swapchain;
  std::vector<std::unique_ptr<RenderFrame>> frames;
  const Queue *queue{nullptr}; // a present supported queue

  ResourceCache resourceCache{};

  VkSemaphore acquiredSemaphore{VK_NULL_HANDLE};
  uint32_t activeFrameIndex{0U};
};
