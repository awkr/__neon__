#include "renderer/image.h"
#include "renderer/device.h"

bool createImage(Image *image, Device *device, VkImage handle,
                 const VkExtent2D &extent, VkFormat format,
                 VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount) {
  image->device = device;
  image->handle = handle;
  image->extent = extent;
  image->format = format;
  image->usage = usage;
  image->sampleCount = sampleCount;
  return true;
}

bool createImage(Image *image, Device *device, const VkExtent2D &extent,
                 VkFormat format, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperty, uint32_t mipLevels,
                 uint32_t arrayLayers, VkSampleCountFlagBits sampleCount,
                 VkImageTiling tiling,
                 std::vector<uint32_t> queueFamilyIndices) {
  VkImageCreateInfo imageCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = format;
  imageCreateInfo.extent = {extent.width, extent.height, 1};
  imageCreateInfo.mipLevels = mipLevels;
  imageCreateInfo.arrayLayers = arrayLayers;
  imageCreateInfo.samples = sampleCount;
  imageCreateInfo.tiling = tiling;
  imageCreateInfo.usage = usage;

  if (!queueFamilyIndices.empty()) {
    imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    imageCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
    imageCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  }

  VkImage handle{VK_NULL_HANDLE};
  if (vkCreateImage(device->handle, &imageCreateInfo, nullptr, &handle) !=
      VK_SUCCESS) {
    return false;
  }

  createImage(image, device, handle, extent, format, usage, sampleCount);

  VkMemoryRequirements memoryRequirements{};
  vkGetImageMemoryRequirements(device->handle, image->handle,
                               &memoryRequirements);

  if (usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
    memoryProperty |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  getMemoryTypeIndex(device, memoryRequirements.memoryTypeBits, memoryProperty,
                     memoryAllocateInfo.memoryTypeIndex);

  VkDeviceMemory memory{VK_NULL_HANDLE};
  if (vkAllocateMemory(device->handle, &memoryAllocateInfo, nullptr, &memory) !=
      VK_SUCCESS) {
    return false;
  }

  if (vkBindImageMemory(device->handle, handle, memory, 0) != VK_SUCCESS) {
    return false;
  }

  image->memory = memory;

  return true;
}

void destroyImage(Image *image) {
  if (image->handle && image->memory) {
    vkFreeMemory(image->device->handle, image->memory, nullptr);
    vkDestroyImage(image->device->handle, image->handle, nullptr);
  }
}
