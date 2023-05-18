#include "renderer/render_target.h"
#include "renderer/device.h"

const RenderTargetCreateFunc RENDER_TARGET_DEFAULT_CREATE_FUNC =
    [](Image color, RenderTarget *renderTarget) -> bool {
  Image depth{};
  auto format = chooseDepthFormat(color.device->physicalDevice);
  auto usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
  if (!createImage(&depth, color.device, color.extent, format, usage,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    return false;
  }

  renderTarget->images.emplace_back(color);
  renderTarget->images.emplace_back(depth);

  for (auto &image : renderTarget->images) {
    ImageView imageView{};
    if (!createImageView(&imageView, &image)) {
      return false;
    }

    renderTarget->imageViews.emplace_back(imageView);
  }

  return true;
};

void destroyRenderTarget(RenderTarget *renderTarget) {
  for (auto &imageView : renderTarget->imageViews) {
    destroyImageView(&imageView);
  }
  renderTarget->imageViews.clear();

  for (auto &image : renderTarget->images) {
    destroyImage(&image);
  }
  renderTarget->images.clear();
}
