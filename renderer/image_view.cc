#include "renderer/image_view.h"
#include "renderer/device.h"

bool createImageView(ImageView *imageView, Image *image) {
  VkImageViewCreateInfo imageViewCreateInfo{
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.image = image->handle;
  imageViewCreateInfo.format = image->format;

  if (isDepthStencilFormat(image->format)) {
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  } else {
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(image->device->handle, &imageViewCreateInfo, nullptr,
                        &imageView->handle) != VK_SUCCESS) {
    return false;
  }

  imageView->image = image;

  return true;
}

void destroyImageView(ImageView *imageView) {
  vkDestroyImageView(imageView->image->device->handle, imageView->handle,
                     nullptr);
}
