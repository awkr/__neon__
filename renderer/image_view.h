#pragma once

#include "renderer/image.h"

struct ImageView {
  Image                  *image{nullptr};
  VkFormat                format{VK_FORMAT_UNDEFINED};
  VkImageView             handle{VK_NULL_HANDLE};
  VkImageSubresourceRange subresourceRange{};
};

bool createImageView(ImageView *imageView, Image *image);

void destroyImageView(ImageView *imageView);
