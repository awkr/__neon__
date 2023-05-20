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

    RenderTarget renderTarget{};
    if (!RENDER_TARGET_DEFAULT_CREATE_FUNC(image, &renderTarget)) {
      return nullptr;
    }

    auto renderFrame = std::make_unique<RenderFrame>(*swapchain->device,
                                                     std::move(renderTarget));
    renderFrames[i] = std::move(renderFrame);
  }

  Queue *queue{nullptr};
  if (!swapchain->device->getGraphicsQueue(&queue)) { return nullptr; }

  auto renderContext = std::make_unique<RenderContext>();
  renderContext->device = swapchain->device;
  renderContext->swapchain = std::move(swapchain);
  renderContext->frames = std::move(renderFrames);
  renderContext->queue = queue;
  return std::move(renderContext);
}

RenderContext::~RenderContext() {
  frames.clear();
  swapchain.reset();
}

bool RenderContext::begin(CommandBuffer **commandBuffer) {
  if (!beginFrame()) { return false; }
  Queue *queue{nullptr};
  if (!device->getQueue(VK_QUEUE_GRAPHICS_BIT, 0, &queue)) { return false; }
  bool ok = getActiveFrame()->requestCommandBuffer(
      commandBuffer, queue, CommandBufferResetMode::ResetPool);
  if (!ok) { return false; }
  return true;
}

bool RenderContext::submit(CommandBuffer *commandBuffer) {
  VkSemaphore renderCompleteSemaphore{VK_NULL_HANDLE};
  if (!submit(queue, {commandBuffer}, acquiredSemaphore,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
              renderCompleteSemaphore)) {
    return false;
  }
  endFrame(renderCompleteSemaphore);
  return true;
}

bool RenderContext::beginFrame() {
  auto frame = getActiveFrame();
  if (!frame->requestOutSemaphore(acquiredSemaphore)) { return false; }

  auto result = swapchain->acquireImage(activeFrameIndex, acquiredSemaphore);
  if (result != VK_SUCCESS) { return false; }

  waitFrame();

  printf("++> acquired image index %d\n", activeFrameIndex);

  return true;
}

void RenderContext::endFrame(VkSemaphore semaphore) {
  VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &semaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain->handle;
  presentInfo.pImageIndices = &activeFrameIndex;

  queue->present(presentInfo);

  getActiveFrame()->releaseSemaphore(acquiredSemaphore);
  acquiredSemaphore = VK_NULL_HANDLE;
}

void RenderContext::waitFrame() { getActiveFrame()->reset(); }

bool RenderContext::submit(const Queue *queue,
                           const std::vector<CommandBuffer *> &commandBuffers,
                           VkSemaphore waitSemaphore,
                           VkPipelineStageFlags waitPipelineStage,
                           VkSemaphore &renderCompleteSemaphore) {
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
  //  submitInfo.commandBufferCount = commandBufferHandles.size();
  //  submitInfo.pCommandBuffers = commandBufferHandles.data();

  if (waitSemaphore) {
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &waitPipelineStage;
  }

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &signalSemaphore;

  VkFence fence{VK_NULL_HANDLE};
  if (!frame->requestFence(fence)) { return false; }

  if (!queue->submit({submitInfo}, fence)) { return false; }

  renderCompleteSemaphore = signalSemaphore;

  return true;
}

RenderFrame *RenderContext::getActiveFrame() {
  return frames[activeFrameIndex].get();
}
