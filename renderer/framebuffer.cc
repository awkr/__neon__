#include "renderer/framebuffer.h"
#include "renderer/device.h"
#include "renderer/render_target.h"

std::unique_ptr<Framebuffer> Framebuffer::make(Device &device,
                                               const RenderTarget &renderTarget,
                                               const RenderPass &renderPass) {
  std::vector<VkImageView> attachments;
  for (const auto &view : renderTarget.imageViews) {
    attachments.emplace_back(view.handle);
  }

  VkFramebufferCreateInfo createInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};

  createInfo.renderPass = renderPass.handle;
  createInfo.attachmentCount = attachments.size();
  createInfo.pAttachments = attachments.data();
  createInfo.width = renderTarget.getExtent().width;
  createInfo.height = renderTarget.getExtent().height;
  createInfo.layers = 1;

  VkFramebuffer handle{VK_NULL_HANDLE};
  if (vkCreateFramebuffer(device.handle, &createInfo, nullptr, &handle) !=
      VK_SUCCESS) {
    return nullptr;
  }

  auto framebuffer = std::make_unique<Framebuffer>(device);
  framebuffer->handle = handle;
  framebuffer->extent = renderTarget.getExtent();
  return framebuffer;
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(device.handle, handle, nullptr);
}
