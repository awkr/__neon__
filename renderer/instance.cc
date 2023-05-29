#include "renderer/instance.h"
#include "core/string_utils.h"
#include <unordered_map>
#include <vulkan/vulkan_metal.h>

VKAPI_ATTR VkBool32 VKAPI_CALL messengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData) {
  if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ||
      (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
    printf("%s\n", callbackData->pMessage);
  }
  return VK_FALSE;
}

bool createInstance(VkInstance *instance, VkDebugUtilsMessengerEXT *messenger) {
  // extension name: optional
  std::unordered_map<const char *, bool> requiredInstanceExtensions;
  requiredInstanceExtensions[VK_EXT_DEBUG_UTILS_EXTENSION_NAME]   = false;
  requiredInstanceExtensions[VK_EXT_METAL_SURFACE_EXTENSION_NAME] = false;
  requiredInstanceExtensions[VK_KHR_SURFACE_EXTENSION_NAME]       = false;
  requiredInstanceExtensions[VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME] =
      true;

  uint32_t instanceExtensionCount;
  vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount,
                                         nullptr);

  std::vector<VkExtensionProperties> availableInstanceExtensions(
      instanceExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount,
                                         availableInstanceExtensions.data());

  std::vector<const char *> enabledInstanceExtensions;

  for (const auto &requiredExtension : requiredInstanceExtensions) {
    bool found;
    for (const auto &availableExtension : availableInstanceExtensions) {
      if (equals(availableExtension.extensionName, requiredExtension.first)) {
        found = true;
        break;
      }
    }
    if (!found) {
      if (!requiredExtension.second) { // not optional
        printf("required instance extension not supported: %s\n",
               requiredExtension.first);
        return false;
      }
    } else {
      enabledInstanceExtensions.emplace_back(requiredExtension.first);
    }
  }

  // layer name: optional
  std::unordered_map<const char *, bool> requiredInstanceLayers;
  requiredInstanceLayers["VK_LAYER_KHRONOS_validation"] = false;

  uint32_t instanceLayerCount;
  vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

  std::vector<VkLayerProperties> availableInstanceLayers(instanceLayerCount);
  vkEnumerateInstanceLayerProperties(&instanceLayerCount,
                                     availableInstanceLayers.data());

  std::vector<const char *> enabledInstanceLayers;

  for (const auto &requiredLayer : requiredInstanceLayers) {
    bool found;
    for (const auto &availableLayer : availableInstanceLayers) {
      if (equals(availableLayer.layerName, requiredLayer.first)) {
        found = true;
        break;
      }
    }
    if (!found) {
      if (!requiredLayer.second) { // not optional
        printf("required instance layer not supported: %s\n",
               requiredLayer.first);
        return false;
      }
    } else {
      enabledInstanceLayers.emplace_back(requiredLayer.first);
    }
  }

  VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};

  appInfo.pApplicationName   = "Neon";
  appInfo.applicationVersion = 0;
  appInfo.pEngineName        = "Neon";
  appInfo.engineVersion      = 0;
  appInfo.apiVersion         = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instanceInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};

  instanceInfo.pApplicationInfo = &appInfo;

  instanceInfo.enabledExtensionCount   = enabledInstanceExtensions.size();
  instanceInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

  instanceInfo.enabledLayerCount   = enabledInstanceLayers.size();
  instanceInfo.ppEnabledLayerNames = enabledInstanceLayers.data();

  VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  messengerCreateInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  messengerCreateInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  messengerCreateInfo.pfnUserCallback = messengerCallback;

  instanceInfo.pNext = &messengerCreateInfo;

  instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  if (vkCreateInstance(&instanceInfo, nullptr, instance) != VK_SUCCESS) {
    return false;
  }

  auto fnVkCreateDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT"));
  if (fnVkCreateDebugUtilsMessengerEXT(*instance, &messengerCreateInfo, nullptr,
                                       messenger) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void destroyInstance(VkInstance               *instance,
                     VkDebugUtilsMessengerEXT *messenger) {
  auto fnVkDestroyDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT"));
  fnVkDestroyDebugUtilsMessengerEXT(*instance, *messenger, nullptr);
  messenger = VK_NULL_HANDLE;

  vkDestroyInstance(*instance, nullptr);
  instance = VK_NULL_HANDLE;
}
