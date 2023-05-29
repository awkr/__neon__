#pragma once

#include "renderer/image_view.h"
#include <functional>

struct Attachment {
  VkFormat              format{VK_FORMAT_UNDEFINED};
  VkImageUsageFlags     usage{};
  VkSampleCountFlagBits sampleCount{VK_SAMPLE_COUNT_1_BIT};
  VkImageLayout         initialLayout{VK_IMAGE_LAYOUT_UNDEFINED}; // TODO
};

struct RenderTarget {
  using CreateFunc = std::function<std::unique_ptr<RenderTarget>(Image color)>;

  static const CreateFunc DEFAULT_CREATE_FUNC;

  ~RenderTarget();

  const std::vector<Attachment> &getAttachments() const { return attachments; }
  // void setInputAttachments(const std::vector<uint32_t> &inputs);
  // void setOutputAttachments(const std::vector<uint32_t> &outputs);

  const VkExtent2D &getExtent() const { return extent; }

  VkExtent2D              extent{};
  std::vector<Image>      images;
  std::vector<ImageView>  imageViews;
  std::vector<Attachment> attachments;

  // // default to no input attachments
  // std::vector<uint32_t> inputAttachments = {};
  // // default to swapchain output attachment
  // std::vector<uint32_t> outputAttachments = {0};
};
