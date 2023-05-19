#pragma once

#include "renderer/render_target.h"
#include "renderer/semaphore_pool.h"

struct RenderFrame {
  RenderFrame(Device &device, RenderTarget &&renderTarget);

  ~RenderFrame();

  bool requestOwnedSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);

  RenderTarget renderTarget{};
  SemaphorePool semaphorePool;
};
