#pragma once

#include "renderer/render_frame.h"
#include "renderer/swapchain.h"

struct RenderContext {
  std::vector<RenderFrame> renderFrames;
};

bool createRenderContext(RenderContext *renderContext, Swapchain *swapchain);

void destroyRenderContext(RenderContext *renderContext);
