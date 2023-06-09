#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct SwapchainProperties {
  VkExtent2D extent{};
  uint16_t imageCount{3U};
  uint32_t imageArrayLayers{1U};
  VkSurfaceFormatKHR surfaceFormat{VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  std::set<VkImageUsageFlagBits> imageUsages{};
  VkSurfaceTransformFlagBitsKHR preTransform{
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};
  VkCompositeAlphaFlagBitsKHR compositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR};
  VkPresentModeKHR presentMode{VK_PRESENT_MODE_FIFO_KHR};
};

struct Swapchain {
  static std::unique_ptr<Swapchain>
  make(Device &device, VkSurfaceKHR surface, const VkExtent2D &extent,
       uint16_t imageCount,
       const std::set<VkImageUsageFlagBits> &imageUsages =
           {
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
           },
       const VkSurfaceFormatKHR &surfaceFormat =
           {
               VK_FORMAT_R8G8B8A8_SRGB,
               VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
           },
       VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR,
       VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

  static std::unique_ptr<Swapchain> make(Swapchain &oldSwapchain,
                                         const VkExtent2D &extent);

  Swapchain(Device *device, VkSurfaceKHR surface);
  Swapchain(const Swapchain &swapchain) = delete;
  ~Swapchain();

  VkResult acquireImage(uint32_t &index, VkSemaphore semaphore);

  Device *device{nullptr};
  VkSurfaceKHR surface{VK_NULL_HANDLE};
  SwapchainProperties properties{};
  VkSwapchainKHR handle{VK_NULL_HANDLE};
  std::vector<VkImage> images;
};
