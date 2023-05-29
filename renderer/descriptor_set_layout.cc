#include "renderer/descriptor_set_layout.h"
#include "renderer/device.h"
#include "renderer/shader_module.h"

bool findDescriptorType(ShaderResourceType type, bool dynamic,
                        VkDescriptorType &descriptorType);

std::unique_ptr<DescriptorSetLayout>
DescriptorSetLayout::make(Device &device, uint32_t setIndex,
                          const std::vector<ShaderModule *> &shaderModules,
                          const std::vector<ShaderResource> &shaderResources) {
  std::vector<VkDescriptorBindingFlagsEXT>  bindingFlags;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
  // Store mapping between binding and the binding point
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindingsLookup;
  std::unordered_map<uint32_t, VkDescriptorBindingFlagsEXT>  bindingFlagsLookup;
  std::unordered_map<std::string, uint32_t>                  resourcesLookup;

  for (const auto &resource : shaderResources) {
    // Skip shader resources without a binding point
    if (resource.type == ShaderResourceType::Input ||
        resource.type == ShaderResourceType::Output ||
        resource.type == ShaderResourceType::PushConstant ||
        resource.type == ShaderResourceType::SpecializationConstant) {
      continue;
    }

    VkDescriptorType descriptorType{};
    if (!findDescriptorType(resource.type,
                            resource.mode == ShaderResourceMode::Dynamic,
                            descriptorType)) {
      return nullptr;
    }

    if (resource.mode == ShaderResourceMode::UpdateAfterBind) {
      bindingFlags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);
    } else {
      bindingFlags.push_back(0);
    }

    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding         = resource.binding;
    layoutBinding.descriptorCount = resource.arraySize;
    layoutBinding.descriptorType  = descriptorType;
    layoutBinding.stageFlags      = resource.stages;

    bindings.emplace_back(layoutBinding);

    bindingsLookup.emplace(resource.binding, layoutBinding);
    bindingFlagsLookup.emplace(resource.binding, bindingFlags.back());
    resourcesLookup.emplace(resource.name, resource.binding);
  }

  VkDescriptorSetLayoutCreateInfo createInfo{
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  createInfo.flags        = 0;
  createInfo.bindingCount = bindings.size();
  createInfo.pBindings    = bindings.data();

  // Handle update-after-bind extensions
  if (std::find_if(shaderResources.begin(), shaderResources.end(),
                   [](const ShaderResource &resource) {
                     return resource.mode ==
                            ShaderResourceMode::UpdateAfterBind;
                   }) != shaderResources.end()) {
    // Spec states you can't have any dynamic resources if you have one of the
    // bindings set to update-after-bind
    if (std::find_if(shaderResources.begin(), shaderResources.end(),
                     [](const ShaderResource &resource) {
                       return resource.mode == ShaderResourceMode::Dynamic;
                     }) != shaderResources.end()) {
      return nullptr;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT};
    bindingFlagsCreateInfo.bindingCount  = bindings.size();
    bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();

    createInfo.pNext = &bindingFlagsCreateInfo;
    createInfo.flags |=
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
  }

  VkDescriptorSetLayout handle{VK_NULL_HANDLE};
  if (vkCreateDescriptorSetLayout(device.handle, &createInfo, nullptr,
                                  &handle) != VK_SUCCESS) {
    return nullptr;
  }

  auto descriptorSetLayout      = std::make_unique<DescriptorSetLayout>(device);
  descriptorSetLayout->handle   = handle;
  descriptorSetLayout->setIndex = setIndex;
  descriptorSetLayout->bindingFlags       = std::move(bindingFlags);
  descriptorSetLayout->bindingsLookup     = std::move(bindingsLookup);
  descriptorSetLayout->bindingFlagsLookup = std::move(bindingFlagsLookup);
  return descriptorSetLayout;
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(device.handle, handle, nullptr);
}

const VkDescriptorSetLayoutBinding *
DescriptorSetLayout::getLayoutBinding(uint32_t bindingIndex) const {
  auto it = bindingsLookup.find(bindingIndex);
  if (it == bindingsLookup.end()) { return nullptr; }
  return &it->second;
}

VkDescriptorBindingFlagsEXT
DescriptorSetLayout::getLayoutBindingFlag(const uint32_t bindingIndex) const {
  auto it = bindingFlagsLookup.find(bindingIndex);
  if (it == bindingFlagsLookup.end()) { return 0; }
  return it->second;
}

bool findDescriptorType(ShaderResourceType type, bool dynamic,
                        VkDescriptorType &descriptorType) {
  switch (type) {
  case ShaderResourceType::InputAttachment:
    descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    return true;
  case ShaderResourceType::Image:
    descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    return true;
  case ShaderResourceType::ImageSampler:
    descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    return true;
  case ShaderResourceType::ImageStorage:
    descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    return true;
  case ShaderResourceType::Sampler:
    descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    return true;
  case ShaderResourceType::BufferUniform:
    if (dynamic) {
      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    } else {
      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
    return true;
  case ShaderResourceType::BufferStorage:
    if (dynamic) {
      descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    } else {
      descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
    return true;
  default:
    return false;
  }
}
