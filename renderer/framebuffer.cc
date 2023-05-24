#include "renderer/framebuffer.h"

std::unique_ptr<Framebuffer> Framebuffer::make(Device &device,
                                               const RenderTarget &renderTarget,
                                               const RenderPass &renderPass) {
  auto framebuffer = std::make_unique<Framebuffer>(device);
  return framebuffer;
}
