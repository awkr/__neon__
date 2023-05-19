#include "renderer/render_context.h"

std::unique_ptr<RenderContext>
RenderContext::make(std::unique_ptr<Swapchain> &&swapchain) {
  std::vector<std::unique_ptr<RenderFrame>> renderFrames(
      swapchain->images.size());

  for (uint32_t i = 0; i < swapchain->images.size(); ++i) {
    Image image{};
    if (!createImage(&image, swapchain->device, swapchain->images[i],
                     swapchain->properties.extent,
                     swapchain->properties.surfaceFormat.format)) {
      return nullptr;
    }

    RenderTarget renderTarget{};
    if (!RENDER_TARGET_DEFAULT_CREATE_FUNC(image, &renderTarget)) {
      return nullptr;
    }

    auto renderFrame = std::make_unique<RenderFrame>(*swapchain->device,
                                                     std::move(renderTarget));
    renderFrames[i] = std::move(renderFrame);
  }

  auto renderContext = std::make_unique<RenderContext>();
  renderContext->device = swapchain->device;
  renderContext->swapchain = std::move(swapchain);
  renderContext->frames = std::move(renderFrames);
  return std::move(renderContext);
}

RenderContext::~RenderContext() {
  frames.clear();
  swapchain.reset();
}

bool RenderContext::begin() {
  if (!beginFrame()) { return false; }
  // CommandBufferResetMode resetMode = CommandBufferResetMode::RestPool
  return true;
}

void RenderContext::submit() { endFrame(); }

bool RenderContext::beginFrame() {
  auto &frame = frames[activeFrameIndex];
  if (!frame->requestOwnedSemaphore(imageAcquiredSemaphore)) { return false; }

  auto result =
      swapchain->acquireImage(activeFrameIndex, imageAcquiredSemaphore);
  if (result != VK_SUCCESS) { return false; }

  waitFrame();

  return true;
}

void RenderContext::endFrame() {
  auto &frame = frames[activeFrameIndex];
  frame->releaseSemaphore(imageAcquiredSemaphore);
  imageAcquiredSemaphore = VK_NULL_HANDLE;
}

void RenderContext::waitFrame() {}
