#pragma once

#include "renderer/physical_device.h"
#include "renderer/queue.h"
#include "renderer/resource_cache.h"

struct Device {
  static std::unique_ptr<Device> make(VkInstance   instance,
                                      VkSurfaceKHR surface);

  Device() : resourceCache(*this) {}
  ~Device();
  bool getQueue(VkQueueFlags requiredFlags, uint32_t index,
                const Queue **queue) const;
  bool waitIdle() const;
  bool getGraphicsQueue(const Queue **queue) const;

  VkDevice getHandle() const { return handle; }

  ResourceCache &getResourceCache() { return resourceCache; }

  PhysicalDevice                  physicalDevice{};
  VkDevice                        handle{VK_NULL_HANDLE};
  std::vector<std::vector<Queue>> queues;

private:
  ResourceCache resourceCache;
};

VkFormat chooseDepthFormat(VkPhysicalDevice             physicalDevice,
                           bool                         depthOnly = false,
                           const std::vector<VkFormat> &depthFormatPriority = {
                               VK_FORMAT_D32_SFLOAT,
                               VK_FORMAT_D24_UNORM_S8_UINT,
                               VK_FORMAT_D16_UNORM,
                           });

bool getMemoryTypeIndex(Device *device, uint32_t typeBits,
                        VkMemoryPropertyFlags property, uint32_t &typeIndex);

bool isDepthOnlyFormat(VkFormat format);
bool isDepthStencilFormat(VkFormat format);
