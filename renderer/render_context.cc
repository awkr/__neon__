#include "renderer/render_context.h"

bool createRenderContext(RenderContext *renderContext, Swapchain *swapchain) {
  for (const auto &handle : swapchain->images) {
    Image image{};
    if (!createImage(&image, swapchain->device, handle,
                     swapchain->properties.extent,
                     swapchain->properties.surfaceFormat.format)) {
      return false;
    }
    auto renderTarget = RENDER_TARGET_DEFAULT_CREATE_FUNC(image);
    renderContext->renderTargets.emplace_back(renderTarget);
  }
  return true;
}

void destroyRenderContext(RenderContext *renderContext) {
  for (auto &renderTarget : renderContext->renderTargets) {
    destroyRenderTarget(renderTarget);
  }
  renderContext->renderTargets.clear();
}
