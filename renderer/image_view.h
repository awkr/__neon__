#pragma once

#include "renderer/image.h"

struct ImageView {
  Image *image{nullptr};
  VkImageView handle{VK_NULL_HANDLE};
  VkImageSubresourceRange subresourceRange{};
};

bool createImageView(ImageView *imageView, Image *image);

void destroyImageView(ImageView *imageView);
