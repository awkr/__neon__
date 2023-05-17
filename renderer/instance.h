#pragma once

#include <vulkan/vulkan.h>

bool createInstance(VkInstance *instance, VkDebugUtilsMessengerEXT *messenger);

void destroyInstance(VkInstance *instance, VkDebugUtilsMessengerEXT *messenger);
