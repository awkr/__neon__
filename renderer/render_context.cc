#include "renderer/render_context.h"
#include "renderer/device.h"

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

    auto renderTarget = RenderTarget::DEFAULT_CREATE_FUNC(image);
    if (!renderTarget) { return nullptr; }

    auto renderFrame = std::make_unique<RenderFrame>(*swapchain->device,
                                                     std::move(renderTarget));
    renderFrames[i] = std::move(renderFrame);
  }

  const Queue *queue{nullptr};
  if (!swapchain->device->getGraphicsQueue(&queue)) { return nullptr; }

  auto renderContext = std::make_unique<RenderContext>();
  renderContext->device = swapchain->device;
  renderContext->swapchain = std::move(swapchain);
  renderContext->frames = std::move(renderFrames);
  renderContext->queue = queue;
  return std::move(renderContext);
}

RenderContext::~RenderContext() {
  resourceCache.clear();
  frames.clear();
  swapchain.reset();
}

bool RenderContext::begin(CommandBuffer **commandBuffer) {
  if (!beginFrame()) { return false; }
  const Queue *graphicsQueue{nullptr};
  if (!device->getQueue(VK_QUEUE_GRAPHICS_BIT, 0, &graphicsQueue)) {
    return false;
  }
  bool ok =
      getActiveFrame()->requestCommandBuffer(commandBuffer, graphicsQueue);
  if (!ok) { return false; }
  return true;
}

bool RenderContext::submit(CommandBuffer *commandBuffer) {
  VkSemaphore renderCompleteSemaphore{VK_NULL_HANDLE};
  if (!submit(*queue, {commandBuffer}, acquiredSemaphore,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
              &renderCompleteSemaphore)) {
    return false;
  }
  if (!endFrame(renderCompleteSemaphore)) { return false; }
  return true;
}

bool RenderContext::beginFrame() {
  if (!handleSurfaceChanges()) { return false; }

  auto frame = getActiveFrame();
  if (!frame->requestOutSemaphore(acquiredSemaphore)) { return false; }

  auto result = swapchain->acquireImage(activeFrameIndex, acquiredSemaphore);
  if (result != VK_SUCCESS) { return false; }

  waitFrame();

  return true;
}

bool RenderContext::endFrame(VkSemaphore waitSemaphore) {
  VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &waitSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain->handle;
  presentInfo.pImageIndices = &activeFrameIndex;

  if (!queue->present(presentInfo)) { return false; }

  // frame is not active anymore, release owned semaphore
  getActiveFrame()->releaseSemaphore(acquiredSemaphore);
  acquiredSemaphore = VK_NULL_HANDLE;
  return true;
}

void RenderContext::waitFrame() { getActiveFrame()->reset(); }

bool RenderContext::submit(const Queue &graphicsQueue,
                           const std::vector<CommandBuffer *> &commandBuffers,
                           VkSemaphore waitSemaphore,
                           VkPipelineStageFlags waitPipelineStage,
                           VkSemaphore *renderCompleteSemaphore) {
  std::vector<VkCommandBuffer> commandBufferHandles(commandBuffers.size(),
                                                    VK_NULL_HANDLE);
  std::transform(
      commandBuffers.begin(), commandBuffers.end(),
      commandBufferHandles.begin(),
      [](const CommandBuffer *commandBuffer) { return commandBuffer->handle; });

  auto frame = getActiveFrame();

  VkSemaphore signalSemaphore{VK_NULL_HANDLE};
  if (!frame->requestSemaphore(signalSemaphore)) { return false; }

  VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.commandBufferCount = commandBufferHandles.size();
  submitInfo.pCommandBuffers = commandBufferHandles.data();

  if (waitSemaphore) {
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &waitPipelineStage;
  }

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &signalSemaphore;

  VkFence fence{VK_NULL_HANDLE};
  if (!frame->requestFence(fence)) { return false; }

  if (!graphicsQueue.submit({submitInfo}, fence)) { return false; }

  *renderCompleteSemaphore = signalSemaphore;

  return true;
}

bool RenderContext::handleSurfaceChanges() {
  VkSurfaceCapabilitiesKHR properties{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice,
                                            swapchain->surface, &properties);
  const auto &currentExtent = properties.currentExtent;
  if (currentExtent.width == 0xffffffff) { return false; }
  const auto &existingExtent = swapchain->properties.extent;
  if (currentExtent.width == existingExtent.width &&
      currentExtent.height == existingExtent.height) {
    return true;
  }
  if (!device->waitIdle()) { return false; }
  resourceCache.clearFramebuffers();
  swapchain = Swapchain::make(*swapchain, currentExtent);
  auto it = frames.begin();
  for (auto imageHandle : swapchain->images) {
    Image image{};
    if (!createImage(&image, device, imageHandle, currentExtent,
                     swapchain->properties.surfaceFormat.format)) {
      return false;
    }

    auto renderTarget = RenderTarget::DEFAULT_CREATE_FUNC(image);
    if (!renderTarget) { return false; }

    if (it != frames.end()) {
      (*it)->updateRenderTarget(std::move(renderTarget));
    } else {
      auto renderFrame = std::make_unique<RenderFrame>(*swapchain->device,
                                                       std::move(renderTarget));
      frames.emplace_back(std::move(renderFrame));
    }

    ++it;
  }
  return true;
}

RenderFrame *RenderContext::getActiveFrame() {
  return frames[activeFrameIndex].get();
}
