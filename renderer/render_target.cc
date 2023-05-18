#include "renderer/render_target.h"
#include "renderer/device.h"

const RenderTargetCreateFunc RENDER_TARGET_DEFAULT_CREATE_FUNC =
    [](Image image) -> RenderTarget * {
  Image depth{};
  auto format = chooseDepthFormat(image.device->physicalDevice);
  auto usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
  if (!createImage(&depth, image.device, image.extent, format, usage,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    return nullptr;
  }

  auto renderTarget = new RenderTarget();
  renderTarget->images.emplace_back(image);
  renderTarget->images.emplace_back(depth);
  return renderTarget;
};

void destroyRenderTarget(RenderTarget *renderTarget) {
  for (auto &image : renderTarget->images) {
    destroyImage(&image);
  }
  renderTarget->images.clear();
}
