#pragma once

#include "renderer/resource.h"
#include <memory>
#include <vulkan/vulkan.h>

struct Device;
struct RenderTarget;
struct RenderPass;

struct Framebuffer : public Resource {
  static std::unique_ptr<Framebuffer> make(Device &device,
                                           const RenderTarget &renderTarget,
                                           const RenderPass &renderPass);

  Framebuffer(Device &device) : device{device} {}

  Device &device;
  VkFramebuffer handle{VK_NULL_HANDLE};
};
