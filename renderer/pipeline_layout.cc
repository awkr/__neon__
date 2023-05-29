#include "renderer/pipeline_layout.h"
#include "renderer/descriptor_set_layout.h"
#include "renderer/device.h"

std::unique_ptr<PipelineLayout>
PipelineLayout::make(Device                            &device,
                     const std::vector<ShaderModule *> &shaderModules) {
  auto pipelineLayout = std::make_unique<PipelineLayout>(device);

  // Collect and combine all the shader resources
  std::unordered_map<std::string, ShaderResource> shaderResources;
  for (const auto shaderModule : shaderModules) {
    for (const auto &resource : shaderModule->getResources()) {
      std::string key;
      if (resource.type == ShaderResourceType::Input ||
          resource.type == ShaderResourceType::Output) {
        key = std::to_string(resource.stages) + "_" + resource.name;
      } else {
        key = resource.name;
      }

      if (auto it = shaderResources.find(key); it != shaderResources.end()) {
        it->second.stages |= resource.stages;
      } else {
        shaderResources.emplace(key, resource);
      }
    }
  }

  pipelineLayout->shaderResources = std::move(shaderResources);
  pipelineLayout->shaderModules   = shaderModules;

  // Separate shader resources into respective sets
  std::unordered_map<uint32_t, std::vector<ShaderResource>> shaderSets;
  for (auto &it : pipelineLayout->shaderResources) {
    const auto &resource = it.second;
    if (auto iter = shaderSets.find(resource.set); iter != shaderSets.end()) {
      iter->second.emplace_back(resource);
    } else {
      shaderSets.emplace(resource.set, std::vector<ShaderResource>{resource});
    }
  }

  std::vector<DescriptorSetLayout *> descriptorSetLayouts(shaderSets.size());
  // Create a descriptor set layout for each shader set in the shader modules
  for (auto &it : shaderSets) {
    auto setLayout = device.getResourceCache().requestDescriptorSetLayout(
        it.first, shaderModules, it.second);
    if (!setLayout) { return nullptr; }
    descriptorSetLayouts.emplace_back(setLayout);
  }

  std::vector<VkDescriptorSetLayout> descriptorSetLayoutHandles(
      descriptorSetLayouts.size());
  for (const auto &descriptorSetLayout : descriptorSetLayouts) {
    descriptorSetLayoutHandles.push_back(descriptorSetLayout->getHandle());
  }

  pipelineLayout->shaderSets           = std::move(shaderSets);
  pipelineLayout->descriptorSetLayouts = std::move(descriptorSetLayouts);

  // Collect all the push constant shader resources
  std::vector<VkPushConstantRange> pushConstantRanges;
  for (const auto &resource :
       pipelineLayout->getShaderResources(ShaderResourceType::PushConstant)) {
    pushConstantRanges.push_back(
        {resource->stages, resource->offset, resource->size});
  }

  VkPipelineLayoutCreateInfo createInfo{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  createInfo.setLayoutCount         = descriptorSetLayoutHandles.size();
  createInfo.pSetLayouts            = descriptorSetLayoutHandles.data();
  createInfo.pushConstantRangeCount = pushConstantRanges.size();
  createInfo.pPushConstantRanges    = pushConstantRanges.data();

  VkPipelineLayout handle{VK_NULL_HANDLE};
  if (vkCreatePipelineLayout(device.getHandle(), &createInfo, nullptr,
                             &handle) != VK_SUCCESS) {
    return nullptr;
  }
  pipelineLayout->handle = handle;

  return pipelineLayout;
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(device.getHandle(), handle, nullptr);
}

bool PipelineLayout::hasDescriptorSetLayout(uint32_t setIndex) const {
  return setIndex < descriptorSetLayouts.size();
}

std::vector<const ShaderResource *>
PipelineLayout::getShaderResources(const ShaderResourceType &type,
                                   VkShaderStageFlagBits     stage) const {
  std::vector<const ShaderResource *> resources;
  for (const auto &it : shaderResources) {
    const auto &resource = it.second;
    if (resource.type == type && ((resource.stages & stage) == stage)) {
      resources.push_back(&resource);
    }
  }
  return resources;
}

VkShaderStageFlags
PipelineLayout::getPushConstantRangeStage(uint32_t size,
                                          uint32_t offset) const {
  VkShaderStageFlags stages{0};
  for (const auto &resource :
       getShaderResources(ShaderResourceType::PushConstant)) {
    if (offset >= resource->offset &&
        (offset + size) <= (resource->offset + resource->size)) {
      stages |= resource->stages;
    }
  }
  return stages;
}

const DescriptorSetLayout *
PipelineLayout::getDescriptorSetLayout(uint32_t setIndex) const {
  for (const auto &descriptorSetLayout : descriptorSetLayouts) {
    if (descriptorSetLayout->getIndex() == setIndex) {
      return descriptorSetLayout;
    }
  }
  return nullptr;
}
