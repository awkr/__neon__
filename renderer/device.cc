#include "renderer/device.h"
#include "core/string_utils.h"
#include "renderer/ostream.h"
#include <iostream>
#include <unordered_map>

bool isDepthOnlyFormat(VkFormat format) {
  return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT;
}

bool isDepthStencilFormat(VkFormat format) {
  return format == VK_FORMAT_D16_UNORM_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT ||
         format == VK_FORMAT_D32_SFLOAT_S8_UINT || isDepthOnlyFormat(format);
}

std::unique_ptr<Device> Device::make(VkInstance instance,
                                     VkSurfaceKHR surface) {
  // pick a physical device
  uint32_t physicalDeviceCount{0};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
  if (physicalDeviceCount < 1) { return nullptr; }

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                             physicalDevices.data());

  auto device = std::make_unique<Device>();

  // just simply pick the first one
  device->physicalDevice = physicalDevices.front();

  // queue create infos
  uint32_t queueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice,
                                           &queueFamilyPropertyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyProperties(
      queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice,
                                           &queueFamilyPropertyCount,
                                           queueFamilyProperties.data());

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(
      queueFamilyPropertyCount, {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
  std::vector<std::vector<float>> queuePriorities(queueFamilyPropertyCount);

  for (uint32_t queueFamilyIndex = 0U;
       queueFamilyIndex < queueFamilyPropertyCount; ++queueFamilyIndex) {
    const auto &queueFamilyProperty = queueFamilyProperties[queueFamilyIndex];

    queuePriorities[queueFamilyIndex].resize(queueFamilyProperty.queueCount,
                                             0.5f);

    auto &queueCreateInfo = queueCreateInfos[queueFamilyIndex];

    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
    queueCreateInfo.pQueuePriorities = queuePriorities[queueFamilyIndex].data();
  }

  // create logical device

  // extension name: optional
  std::unordered_map<const char *, bool> requiredDeviceExtensions;
  requiredDeviceExtensions[VK_KHR_SWAPCHAIN_EXTENSION_NAME] = false;
  requiredDeviceExtensions["VK_KHR_portability_subset"] = true;

  uint32_t deviceExtensionCount;
  vkEnumerateDeviceExtensionProperties(device->physicalDevice, nullptr,
                                       &deviceExtensionCount, nullptr);

  std::vector<VkExtensionProperties> availableDeviceExtensions(
      deviceExtensionCount);
  vkEnumerateDeviceExtensionProperties(device->physicalDevice, nullptr,
                                       &deviceExtensionCount,
                                       availableDeviceExtensions.data());

  std::vector<const char *> enabledDeviceExtensions;

  for (const auto &requiredExtension : requiredDeviceExtensions) {
    bool found;
    for (const auto &availableExtension : availableDeviceExtensions) {
      if (equals(availableExtension.extensionName, requiredExtension.first)) {
        found = true;
        break;
      }
    }
    if (!found) {
      if (!requiredExtension.second) { // not optional
        printf("required device extension not supported: %s\n",
               requiredExtension.first);
        return nullptr;
      }
    } else {
      enabledDeviceExtensions.emplace_back(requiredExtension.first);
    }
  }

  VkDeviceCreateInfo createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = queueCreateInfos.size();
  createInfo.enabledExtensionCount = enabledDeviceExtensions.size();
  createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

  VkPhysicalDeviceFeatures features{.samplerAnisotropy = VK_TRUE};
  createInfo.pEnabledFeatures = &features;

  if (vkCreateDevice(device->physicalDevice, &createInfo, nullptr,
                     &device->handle) != VK_SUCCESS) {
    return nullptr;
  }

  // create queues
  device->queues.resize(queueFamilyPropertyCount);
  for (uint32_t queueFamilyIndex = 0U;
       queueFamilyIndex < queueFamilyPropertyCount; ++queueFamilyIndex) {
    const auto &queueFamilyProperty = queueFamilyProperties[queueFamilyIndex];
    for (uint32_t queueIndex = 0U; queueIndex < queueFamilyProperty.queueCount;
         ++queueIndex) {
      VkQueue handle{VK_NULL_HANDLE};
      vkGetDeviceQueue(device->handle, queueFamilyIndex, queueIndex, &handle);

      VkBool32 supportPresent{VK_FALSE};
      vkGetPhysicalDeviceSurfaceSupportKHR(
          device->physicalDevice, queueFamilyIndex, surface, &supportPresent);

      Queue queue{
          .handle = handle,
          .familyIndex = queueFamilyIndex,
          .properties = queueFamilyProperty,
          .supportPresent = supportPresent,
      };
      device->queues[queueFamilyIndex].emplace_back(queue);
    }
  }

  return std::move(device);
}

Device::~Device() {
  resourceCache.clear();
  queues.clear();
  vkDestroyDevice(handle, nullptr);
}

VkFormat chooseDepthFormat(VkPhysicalDevice physicalDevice, bool depthOnly,
                           const std::vector<VkFormat> &depthFormatPriority) {
  VkFormat depthFormat{VK_FORMAT_UNDEFINED};

  for (auto &format : depthFormatPriority) {
    if (depthOnly && !isDepthOnlyFormat(format)) { continue; }

    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

    // format must support depth stencil attachment for optimal tiling
    if (properties.optimalTilingFeatures &
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      depthFormat = format;
      break;
    }
  }

  if (depthFormat != VK_FORMAT_UNDEFINED) {
    std::cout << "[Device] Depth format selected " << depthFormat << std::endl;
    return depthFormat;
  }

  throw std::runtime_error(
      "[Device] No suitable depth format could be determined");
}

bool getMemoryTypeIndex(Device *device, uint32_t typeBits,
                        VkMemoryPropertyFlags property, uint32_t &typeIndex) {
  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(device->physicalDevice,
                                      &memoryProperties);

  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
    if ((typeBits & 1) == 1) {
      if ((memoryProperties.memoryTypes[i].propertyFlags & property) ==
          property) {
        typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  return false;
}

bool Device::getQueue(VkQueueFlags requiredFlags, uint32_t index,
                      const Queue **queue) const {
  for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < queues.size();
       ++queueFamilyIndex) {
    const auto &q = queues[queueFamilyIndex][0];

    auto queueFlags = q.properties.queueFlags;
    uint32_t queueCount = q.properties.queueCount;

    if (((queueFlags & requiredFlags) == requiredFlags) && index < queueCount) {
      *queue = &queues[queueFamilyIndex][index];
      return true;
    }
  }

  return false;
}

bool Device::waitIdle() const { return vkDeviceWaitIdle(handle) == VK_SUCCESS; }

bool Device::getGraphicsQueue(const Queue **queue) const {
  for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queues.size();
       ++queueFamilyIndex) {
    auto &q = queues[queueFamilyIndex].front();
    if (q.supportPresent && q.properties.queueCount > 0) {
      *queue = &q;
      return true;
    }
  }
  return getQueue(VK_QUEUE_GRAPHICS_BIT, 0, queue);
}
