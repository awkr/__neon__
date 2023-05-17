#include "renderer/device.h"
#include "core/string_utils.h"
#include <unordered_map>

bool createDevice(VkInstance instance, Device *device) {
  // pick a physical device
  uint32_t physicalDeviceCount{0};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
  if (physicalDeviceCount < 1) {
    return false;
  }

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                             physicalDevices.data());

  device->physicalDevice =
      physicalDevices.front(); // just simply pick the first one

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
      if (stringEquals(availableExtension.extensionName,
                       requiredExtension.first)) {
        found = true;
        break;
      }
    }
    if (!found) {
      if (!requiredExtension.second) { // not optional
        printf("required device extension not supported: %s\n",
               requiredExtension.first);
        return false;
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
    return false;
  }

  // create queues
  device->queues.resize(queueFamilyPropertyCount);
  for (uint32_t queueFamilyIndex = 0U;
       queueFamilyIndex < queueFamilyPropertyCount; ++queueFamilyIndex) {
    const auto &queueFamilyProperty = queueFamilyProperties[queueFamilyIndex];
    for (uint32_t queueIndex = 0U; queueIndex < queueFamilyProperty.queueCount;
         ++queueIndex) {
      VkQueue queue{VK_NULL_HANDLE};
      vkGetDeviceQueue(device->handle, queueFamilyIndex, queueIndex, &queue);

      device->queues[queueFamilyIndex].emplace_back(queue);
    }
  }

  return true;
}

void destroyDevice(Device *device) {
  device->queues.clear();
  vkDestroyDevice(device->handle, nullptr);
  device->handle = VK_NULL_HANDLE;
  device->physicalDevice = VK_NULL_HANDLE;
}
