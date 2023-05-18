#include "renderer/render_context.h"

bool createRenderContext(RenderContext *renderContext, Swapchain *swapchain) {
  renderContext->device = swapchain->device;
  renderContext->renderFrames.resize(swapchain->images.size());

  for (uint32_t i = 0; i < swapchain->images.size(); ++i) {
    Image image{};
    if (!createImage(&image, swapchain->device, swapchain->images[i],
                     swapchain->properties.extent,
                     swapchain->properties.surfaceFormat.format)) {
      return false;
    }

    RenderTarget renderTarget{};
    if (!RENDER_TARGET_DEFAULT_CREATE_FUNC(image, &renderTarget)) {
      return false;
    }

    RenderFrame renderFrame{.renderTarget = std::move(renderTarget)};
    renderContext->renderFrames[i] = std::move(renderFrame);
  }
  return true;
}

void destroyRenderContext(RenderContext *renderContext) {
  for (auto &renderFrame : renderContext->renderFrames) {
    destroyRenderFrame(&renderFrame);
  }
  renderContext->renderFrames.clear();
}
