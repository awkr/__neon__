#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct Image {
  Device               *device{nullptr};
  VkImage               handle{VK_NULL_HANDLE};
  VkExtent2D            extent{};
  VkFormat              format{VK_FORMAT_UNDEFINED};
  VkImageUsageFlags     usage{};
  VkSampleCountFlagBits sampleCount{VK_SAMPLE_COUNT_1_BIT};
  VkDeviceMemory        memory{VK_NULL_HANDLE};
};

bool createImage(Image *image, Device *device, VkImage handle,
                 const VkExtent2D &extent, VkFormat format,
                 VkImageUsageFlags     usage,
                 VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

bool createImage(Image *image, Device *device, const VkExtent2D &extent,
                 VkFormat format, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperty, uint32_t mipLevels = 1,
                 uint32_t              arrayLayers = 1,
                 VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT,
                 VkImageTiling         tiling      = VK_IMAGE_TILING_OPTIMAL,
                 std::vector<uint32_t> queueFamilyIndices = {});

void destroyImage(Image *image);
