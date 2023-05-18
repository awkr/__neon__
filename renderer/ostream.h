#pragma once

#include <ostream>
#include <vulkan/vulkan.h>

std::ostream &operator<<(std::ostream &os, const VkExtent2D &extent);

std::ostream &operator<<(std::ostream &os, const VkFormat &format);

std::ostream &operator<<(std::ostream &os,
                         const VkSurfaceFormatKHR &surfaceFormat);

std::ostream &operator<<(std::ostream &os,
                         VkSurfaceTransformFlagBitsKHR surfaceTransform);

std::ostream &operator<<(std::ostream &os,
                         VkCompositeAlphaFlagBitsKHR compositeAlpha);
