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
  void submit(CommandBuffer *commandBuffer);

  ResourceCache &getResourceCache() { return resourceCache; }

private:
  bool beginFrame();
  void endFrame();

  void waitFrame();

  RenderFrame *getActiveFrame();

  Device *device;
  std::unique_ptr<Swapchain> swapchain;
  std::vector<std::unique_ptr<RenderFrame>> frames;

  ResourceCache resourceCache{};

  VkSemaphore acquiredSemaphore{VK_NULL_HANDLE};
  uint32_t activeFrameIndex{0U};
};
