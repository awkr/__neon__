#include "renderer/render_target.h"
#include "renderer/device.h"

const RenderTarget::CreateFunc RenderTarget::DEFAULT_CREATE_FUNC =
    [](Image color) -> std::unique_ptr<RenderTarget> {
  Image depth{};
  auto format = chooseDepthFormat(color.device->physicalDevice);
  auto usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
  if (!createImage(&depth, color.device, color.extent, format, usage,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
    return nullptr;
  }

  auto renderTarget = std::make_unique<RenderTarget>();
  renderTarget->extent = color.extent;
  renderTarget->images.emplace_back(color);
  renderTarget->images.emplace_back(depth);

  for (auto &image : renderTarget->images) {
    ImageView imageView{};
    if (!createImageView(&imageView, &image)) { return nullptr; }

    renderTarget->imageViews.emplace_back(imageView);
  }

  return std::move(renderTarget);
};

RenderTarget::~RenderTarget() {
  for (auto &imageView : imageViews) { destroyImageView(&imageView); }
  imageViews.clear();

  for (auto &image : images) { destroyImage(&image); }
  images.clear();
}
