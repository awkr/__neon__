#include "renderer/render_frame.h"

RenderFrame::RenderFrame(Device &device, RenderTarget &&renderTarget)
    : semaphorePool{device}, renderTarget{std::move(renderTarget)} {}

RenderFrame::~RenderFrame() { destroyRenderTarget(&renderTarget); }

bool RenderFrame::requestOwnedSemaphore(VkSemaphore &semaphore) {
  return semaphorePool.requestOwnedSemaphore(semaphore);
}

void RenderFrame::releaseSemaphore(VkSemaphore semaphore) {
  semaphorePool.releaseSemaphore(semaphore);
}
