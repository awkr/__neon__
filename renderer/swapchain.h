#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan.h>

struct Device;

struct Swapchain {
  VkSwapchainKHR handle{VK_NULL_HANDLE};
  std::vector<VkImage> images;
};

bool createSwapchain(Swapchain *swapchain, Device *device, VkSurfaceKHR surface,
                     const VkExtent2D &extent, uint16_t imageCount,
                     const std::set<VkImageUsageFlagBits> &imageUsages =
                         {
                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                             VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                         },
                     VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR);

void destroySwapchain(VkDevice device, Swapchain *swapchain);
