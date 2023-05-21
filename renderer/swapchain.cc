#include "renderer/swapchain.h"
#include "renderer/device.h"
#include "renderer/ostream.h"
#include <iostream>

VkExtent2D chooseExtent(const VkExtent2D &requestExtent,
                        const VkExtent2D &minExtent,
                        const VkExtent2D &maxExtent,
                        const VkExtent2D &currentExtent);

uint32_t chooseImageCount(uint32_t requestImageCount, uint32_t minImageCount,
                          uint32_t maxImageCount);

uint32_t chooseImageArrayLayers(uint32_t requestImageArrayLayers,
                                uint32_t maxImageArrayLayers);

VkSurfaceFormatKHR chooseSurfaceFormat(
    const VkSurfaceFormatKHR &requestedSurfaceFormat,
    const std::vector<VkSurfaceFormatKHR> &availableSurfaceFormats,
    const std::vector<VkSurfaceFormatKHR> &surfaceFormatPriority);

bool validateFormatFeature(VkImageUsageFlagBits imageUsage,
                           VkFormatFeatureFlags supportedFeatures);

std::set<VkImageUsageFlagBits>
chooseImageUsage(const std::set<VkImageUsageFlagBits> &requestedImageUsages,
                 VkImageUsageFlags supportedImageUsage,
                 VkFormatFeatureFlags supportedFeatures);

VkImageUsageFlags
compositeImageUsages(const std::set<VkImageUsageFlagBits> &imageUsages);

VkSurfaceTransformFlagBitsKHR
chooseTransform(VkSurfaceTransformFlagBitsKHR requestTransform,
                VkSurfaceTransformFlagsKHR supportedTransform,
                VkSurfaceTransformFlagBitsKHR currentTransform);

VkCompositeAlphaFlagBitsKHR
chooseCompositeAlpha(VkCompositeAlphaFlagBitsKHR requestCompositeAlpha,
                     VkCompositeAlphaFlagsKHR supportedCompositeAlpha);

VkExtent2D chooseExtent(const VkExtent2D &requestExtent,
                        const VkExtent2D &minExtent,
                        const VkExtent2D &maxExtent,
                        const VkExtent2D &currentExtent) {
  if (requestExtent.width < 1 || requestExtent.height < 1) {
    std::cerr << "[Swapchain] extent " << requestExtent
              << " not supported. select " << currentExtent << std::endl;
    return currentExtent;
  }

  if (currentExtent.width == 0xFFFFFFFF) { return requestExtent; }

  VkExtent2D extent{};

  extent.width = std::max(requestExtent.width, minExtent.width);
  extent.width = std::min(requestExtent.width, maxExtent.width);

  extent.height = std::max(requestExtent.height, minExtent.height);
  extent.height = std::min(requestExtent.height, maxExtent.height);

  return extent;
}

uint32_t chooseImageCount(uint32_t requestImageCount, uint32_t minImageCount,
                          uint32_t maxImageCount) {
  if (maxImageCount != 0) {
    requestImageCount = std::min(requestImageCount, maxImageCount);
  }
  requestImageCount = std::max(requestImageCount, minImageCount);
  return requestImageCount;
}

uint32_t chooseImageArrayLayers(uint32_t requestImageArrayLayers,
                                uint32_t maxImageArrayLayers) {
  requestImageArrayLayers =
      std::min(requestImageArrayLayers, maxImageArrayLayers);
  requestImageArrayLayers = std::max(requestImageArrayLayers, 1u);
  return requestImageArrayLayers;
}

VkSurfaceFormatKHR chooseSurfaceFormat(
    const VkSurfaceFormatKHR &requestedSurfaceFormat,
    const std::vector<VkSurfaceFormatKHR> &availableSurfaceFormats,
    const std::vector<VkSurfaceFormatKHR> &surfaceFormatPriority) {
  auto it = std::find_if(
      availableSurfaceFormats.begin(), availableSurfaceFormats.end(),
      [&requestedSurfaceFormat](const VkSurfaceFormatKHR &availableFormat) {
        if (availableFormat.format == requestedSurfaceFormat.format &&
            availableFormat.colorSpace == requestedSurfaceFormat.colorSpace) {
          return true;
        }
        return false;
      });
  if (it == availableSurfaceFormats.end()) {
    for (const auto &preferredSurfaceFormat : surfaceFormatPriority) {
      it = std::find_if(
          availableSurfaceFormats.begin(), availableSurfaceFormats.end(),
          [&preferredSurfaceFormat](const VkSurfaceFormatKHR &availableFormat) {
            if (availableFormat.format == preferredSurfaceFormat.format &&
                availableFormat.colorSpace ==
                    preferredSurfaceFormat.colorSpace) {
              return true;
            }
            return false;
          });
      if (it != availableSurfaceFormats.end()) {
        std::cout << "[Swapchain] Surface format " << requestedSurfaceFormat
                  << " not supported. select " << *it << std::endl;
        return *it;
      }
    }

    it = availableSurfaceFormats.begin();
    std::cout << "[Swapchain] Surface format " << requestedSurfaceFormat
              << " not supported. default to " << *it << std::endl;
  }

  return *it;
}

bool validateFormatFeature(VkImageUsageFlagBits imageUsage,
                           VkFormatFeatureFlags supportedFeatures) {
  switch (imageUsage) {
  case VK_IMAGE_USAGE_STORAGE_BIT:
    return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supportedFeatures;
  default:
    return true;
  }
}

std::set<VkImageUsageFlagBits>
chooseImageUsage(const std::set<VkImageUsageFlagBits> &requestedImageUsages,
                 VkImageUsageFlags supportedImageUsage,
                 VkFormatFeatureFlags supportedFeatures) {
  std::set<VkImageUsageFlagBits> validatedImageUsages;
  for (auto requestedImageUsage : requestedImageUsages) {
    if ((requestedImageUsage & supportedImageUsage) &&
        validateFormatFeature(requestedImageUsage, supportedFeatures)) {
      validatedImageUsages.insert(requestedImageUsage);
    } else {
      std::cout << "[Swapchain] image usage " << requestedImageUsage
                << " requested but not supported" << std::endl;
    }
  }

  if (validatedImageUsages.empty()) {
    // pick the first format from list of defaults, if supported
    const std::vector<VkImageUsageFlagBits> defaultImageUsages = {
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_STORAGE_BIT,
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    };

    for (auto defaultImageUsage : defaultImageUsages) {
      if ((defaultImageUsage & supportedImageUsage) &&
          validateFormatFeature(defaultImageUsage, supportedFeatures)) {
        validatedImageUsages.insert(defaultImageUsage);
        break;
      }
    }
  }

  if (validatedImageUsages.empty()) {
    std::cerr << "[Swapchain] no compatible image usage found" << std::endl;
  }

  return validatedImageUsages;
}

VkImageUsageFlags
compositeImageUsages(const std::set<VkImageUsageFlagBits> &imageUsages) {
  VkImageUsageFlags imageUsage{};
  for (auto usage : imageUsages) { imageUsage |= usage; }
  return imageUsage;
}

VkSurfaceTransformFlagBitsKHR
chooseTransform(VkSurfaceTransformFlagBitsKHR requestTransform,
                VkSurfaceTransformFlagsKHR supportedTransform,
                VkSurfaceTransformFlagBitsKHR currentTransform) {
  if (requestTransform & supportedTransform) { return requestTransform; }

  std::cout << "[Swapchain] surface transform " << requestTransform
            << " not supported. select " << currentTransform;

  return currentTransform;
}

VkCompositeAlphaFlagBitsKHR
chooseCompositeAlpha(VkCompositeAlphaFlagBitsKHR requestCompositeAlpha,
                     VkCompositeAlphaFlagsKHR supportedCompositeAlpha) {
  if (requestCompositeAlpha & supportedCompositeAlpha) {
    return requestCompositeAlpha;
  }

  const std::vector<VkCompositeAlphaFlagBitsKHR> defaultCompositeAlphaFlags = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };

  for (auto compositeAlpha : defaultCompositeAlphaFlags) {
    if (compositeAlpha & supportedCompositeAlpha) {
      std::cout << "[Swapchain] composite alpha " << requestCompositeAlpha
                << " not supported. default to " << compositeAlpha << std::endl;
      return compositeAlpha;
    }
  }

  throw std::runtime_error("no compatible composite alpha found");
}

std::unique_ptr<Swapchain>
Swapchain::make(Device &device, VkSurfaceKHR surface, const VkExtent2D &extent,
                uint16_t imageCount,
                const std::set<VkImageUsageFlagBits> &imageUsages,
                const VkSurfaceFormatKHR &surfaceFormat,
                VkPresentModeKHR presentMode, VkSwapchainKHR oldSwapchain) {
  const std::vector<VkSurfaceFormatKHR> surfaceFormatPriority{
      {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
      {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
  };

  const std::vector<VkPresentModeKHR> presentModePriority{
      VK_PRESENT_MODE_MAILBOX_KHR,
      VK_PRESENT_MODE_FIFO_KHR,
      VK_PRESENT_MODE_IMMEDIATE_KHR,
  };

  uint32_t surfaceFormatCount{0U};
  vkGetPhysicalDeviceSurfaceFormatsKHR(device.physicalDevice, surface,
                                       &surfaceFormatCount, nullptr);

  std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device.physicalDevice, surface,
                                       &surfaceFormatCount,
                                       surfaceFormats.data());

  uint32_t presentModeCount{0U};
  vkGetPhysicalDeviceSurfacePresentModesKHR(device.physicalDevice, surface,
                                            &presentModeCount, nullptr);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      device.physicalDevice, surface, &presentModeCount, presentModes.data());

  VkSurfaceCapabilitiesKHR surfaceCapabilities{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physicalDevice, surface,
                                            &surfaceCapabilities);

  SwapchainProperties properties{};

  properties.extent = chooseExtent(extent, surfaceCapabilities.minImageExtent,
                                   surfaceCapabilities.maxImageExtent,
                                   surfaceCapabilities.currentExtent);

  properties.imageCount =
      chooseImageCount(imageCount, surfaceCapabilities.minImageCount,
                       surfaceCapabilities.maxImageCount);

  properties.imageArrayLayers =
      chooseImageArrayLayers(1u, surfaceCapabilities.maxImageArrayLayers);

  properties.surfaceFormat =
      chooseSurfaceFormat(surfaceFormat, surfaceFormats, surfaceFormatPriority);

  VkFormatProperties formatProperties{};
  vkGetPhysicalDeviceFormatProperties(device.physicalDevice,
                                      properties.surfaceFormat.format,
                                      &formatProperties);

  properties.imageUsages = std::move(
      chooseImageUsage(imageUsages, surfaceCapabilities.supportedUsageFlags,
                       formatProperties.optimalTilingFeatures));

  properties.preTransform =
      chooseTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                      surfaceCapabilities.supportedTransforms,
                      surfaceCapabilities.currentTransform);

  properties.compositeAlpha =
      chooseCompositeAlpha(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
                           surfaceCapabilities.supportedCompositeAlpha);

  properties.presentMode = presentMode;

  //
  VkSwapchainCreateInfoKHR createInfo{
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  createInfo.minImageCount = properties.imageCount;
  createInfo.imageExtent = properties.extent;
  createInfo.presentMode = properties.presentMode;
  createInfo.imageFormat = properties.surfaceFormat.format;
  createInfo.imageColorSpace = properties.surfaceFormat.colorSpace;
  createInfo.imageArrayLayers = properties.imageArrayLayers;
  createInfo.imageUsage = compositeImageUsages(properties.imageUsages);
  createInfo.preTransform = properties.preTransform;
  createInfo.compositeAlpha = properties.compositeAlpha;
  createInfo.oldSwapchain = oldSwapchain;
  createInfo.surface = surface;

  VkSwapchainKHR handle{VK_NULL_HANDLE};
  if (vkCreateSwapchainKHR(device.handle, &createInfo, nullptr, &handle) !=
      VK_SUCCESS) {
    return nullptr;
  }

  auto swapchain = std::make_unique<Swapchain>(&device, surface);
  swapchain->handle = handle;
  swapchain->properties = properties;

  uint32_t imageAvailable{0U};
  vkGetSwapchainImagesKHR(device.handle, swapchain->handle, &imageAvailable,
                          nullptr);

  swapchain->images.resize(imageAvailable);
  vkGetSwapchainImagesKHR(device.handle, swapchain->handle, &imageAvailable,
                          swapchain->images.data());

  return std::move(swapchain);
}

std::unique_ptr<Swapchain> Swapchain::make(Swapchain &oldSwapchain,
                                           const VkExtent2D &extent) {
  return Swapchain::make(*oldSwapchain.device, oldSwapchain.surface, extent,
                         oldSwapchain.properties.imageCount,
                         oldSwapchain.properties.imageUsages,
                         oldSwapchain.properties.surfaceFormat,
                         oldSwapchain.properties.presentMode);
}

Swapchain::Swapchain(Device *device, VkSurfaceKHR surface)
    : device{device}, surface{surface} {}

Swapchain::~Swapchain() {
  images.clear();
  vkDestroySwapchainKHR(device->handle, handle, nullptr);
}

VkResult Swapchain::acquireImage(uint32_t &index, VkSemaphore semaphore) {
  return vkAcquireNextImageKHR(device->handle, handle, UINT64_MAX, semaphore,
                               VK_NULL_HANDLE, &index);
}
