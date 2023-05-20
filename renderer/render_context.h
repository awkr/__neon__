#pragma once

#include "renderer/command_buffer.h"
#include "renderer/render_frame.h"
#include "renderer/resource_cache.h"
#include "renderer/swapchain.h"

#define out **

struct RenderContext {
public:
  static std::unique_ptr<RenderContext>
  make(std::unique_ptr<Swapchain> &&swapchain);

  ~RenderContext();

  bool begin(CommandBuffer **commandBuffer);
  //  bool begin(CommandBuffer out commandBuffer);
  bool submit(CommandBuffer *commandBuffer);

  ResourceCache &getResourceCache() { return resourceCache; }

private:
  bool beginFrame();
  void endFrame(VkSemaphore semaphore);

  void waitFrame();

  bool submit(const Queue *queue,
              const std::vector<CommandBuffer *> &commandBuffers,
              VkSemaphore waitSemaphore, VkPipelineStageFlags waitPipelineStage,
              VkSemaphore &renderCompleteSemaphore);

  RenderFrame *getActiveFrame();

  Device *device;
  std::unique_ptr<Swapchain> swapchain;
  std::vector<std::unique_ptr<RenderFrame>> frames;
  Queue *queue{nullptr};

  ResourceCache resourceCache{};

  VkSemaphore acquiredSemaphore{VK_NULL_HANDLE};
  uint32_t activeFrameIndex{0U};
};
