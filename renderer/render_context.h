#pragma once

#include "renderer/render_target.h"
#include "renderer/swapchain.h"

struct RenderContext {
  std::vector<RenderTarget *> renderTargets;
};

bool createRenderContext(RenderContext *renderContext, Swapchain *swapchain);

void destroyRenderContext(RenderContext *renderContext);
