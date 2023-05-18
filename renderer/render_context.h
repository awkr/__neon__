#pragma once

#include "renderer/render_frame.h"
#include "renderer/resource_cache.h"
#include "renderer/swapchain.h"

struct RenderContext {
  Device *device{nullptr};
  std::vector<RenderFrame> renderFrames;
  ResourceCache resourceCache{};
};

bool createRenderContext(RenderContext *renderContext, Swapchain *swapchain);

void destroyRenderContext(RenderContext *renderContext);
