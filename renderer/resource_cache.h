#pragma once

#include "renderer/descriptor_pool.h"
#include "renderer/descriptor_set.h"
#include "renderer/descriptor_set_layout.h"
#include "renderer/framebuffer.h"
#include "renderer/pipeline.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/shader_module.h"
#include <glm/gtx/hash.hpp>
#include <mutex>

template <typename T> inline void hashParam(size_t &seed, const T &value) {
  std::hash<T> hasher{};
  glm::detail::hash_combine(seed, hasher(value));
}

template <typename T, typename... Args>
inline void hashParam(size_t &seed, const T &arg, const Args &...args) {
  hashParam(seed, arg);
  hashParam(seed, args...);
}

template <>
inline void
hashParam<std::vector<Attachment>>(size_t                        &seed,
                                   const std::vector<Attachment> &value) {
  for (const auto &attachment : value) { hashParam(seed, attachment); }
}

template <>
inline void
hashParam<std::vector<LoadStoreOp>>(size_t                         &seed,
                                    const std::vector<LoadStoreOp> &value) {
  for (const auto &loadStoreInfo : value) { hashParam(seed, loadStoreInfo); }
}

template <>
inline void
hashParam<std::vector<SubpassInfo>>(size_t                         &seed,
                                    const std::vector<SubpassInfo> &value) {
  for (const auto &subpassInfo : value) { hashParam(seed, subpassInfo); }
}

template <>
inline void hashParam<std::vector<ShaderModule *>>(
    size_t &seed, const std::vector<ShaderModule *> &value) {
  for (const auto &shaderModule : value) {
    hashParam(seed, shaderModule->getId());
  }
}

template <>
inline void hashParam<std::vector<ShaderResource>>(
    size_t &seed, const std::vector<ShaderResource> &value) {
  for (const auto &resource : value) { hashParam(seed, resource); }
}

template <>
inline void
hashParam<std::map<uint32_t, std::map<uint32_t, VkDescriptorBufferInfo>>>(
    size_t &seed,
    const std::map<uint32_t, std::map<uint32_t, VkDescriptorBufferInfo>>
        &value) {
  for (auto &bindingSet : value) {
    hashParam(seed, bindingSet.first);

    for (auto &bindingElement : bindingSet.second) {
      hashParam(seed, bindingElement.first);
      hashParam(seed, bindingElement.second);
    }
  }
}

template <>
inline void
hashParam<std::map<uint32_t, std::map<uint32_t, VkDescriptorImageInfo>>>(
    size_t &seed,
    const std::map<uint32_t, std::map<uint32_t, VkDescriptorImageInfo>>
        &value) {
  for (auto &bindingSet : value) {
    hashParam(seed, bindingSet.first);

    for (auto &bindingElement : bindingSet.second) {
      hashParam(seed, bindingElement.first);
      hashParam(seed, bindingElement.second);
    }
  }
}

namespace std {

template <> struct hash<ShaderSource> {
  std::size_t operator()(const ShaderSource &source) const {
    std::size_t result{0};
    hashParam(result, source.id);
    return result;
  }
};

template <> struct hash<ShaderVariant> {
  std::size_t operator()(const ShaderVariant &variant) const {
    std::size_t result{0};
    hashParam(result, variant.getId());
    return result;
  }
};

template <> struct hash<ShaderResource> {
  std::size_t operator()(const ShaderResource &resource) const {
    std::size_t result = 0;

    if (resource.type == ShaderResourceType::Input ||
        resource.type == ShaderResourceType::Output ||
        resource.type == ShaderResourceType::PushConstant ||
        resource.type == ShaderResourceType::SpecializationConstant) {
      return result;
    }

    hashParam(result, resource.set);
    hashParam(result, resource.binding);
    hashParam(result,
              static_cast<std::underlying_type<ShaderResourceType>::type>(
                  resource.type));
    hashParam(result, resource.mode);

    return result;
  }
};

template <> struct hash<Attachment> {
  std::size_t operator()(const Attachment &attachment) const {
    std::size_t result = 0;

    hashParam(result, static_cast<std::underlying_type<VkFormat>::type>(
                          attachment.format));
    hashParam(result,
              static_cast<std::underlying_type<VkSampleCountFlagBits>::type>(
                  attachment.sampleCount));
    hashParam(result, attachment.usage);
    hashParam(result, static_cast<std::underlying_type<VkImageLayout>::type>(
                          attachment.initialLayout));

    return result;
  }
};

template <> struct hash<LoadStoreOp> {
  std::size_t operator()(const LoadStoreOp &load_store_info) const {
    std::size_t result = 0;

    hashParam(result,
              static_cast<std::underlying_type<VkAttachmentLoadOp>::type>(
                  load_store_info.loadOp));
    hashParam(result,
              static_cast<std::underlying_type<VkAttachmentStoreOp>::type>(
                  load_store_info.storeOp));

    return result;
  }
};

template <> struct hash<SubpassInfo> {
  std::size_t operator()(const SubpassInfo &subpassInfo) const {
    std::size_t result = 0;

    for (auto output : subpassInfo.outputAttachments) {
      hashParam(result, output);
    }
    for (auto input : subpassInfo.inputAttachments) {
      hashParam(result, input);
    }
    hashParam(result, subpassInfo.enableDepthStencilAttachment);

    return result;
  }
};

template <> struct hash<RenderTarget> {
  std::size_t operator()(const RenderTarget &renderTarget) const {
    std::size_t result = 0;
    for (const auto &imageView : renderTarget.imageViews) {
      hashParam(result, imageView.handle);
      hashParam(result, imageView.image->handle);
    }
    return result;
  }
};

template <> struct hash<RenderPass> {
  std::size_t operator()(const RenderPass &renderPass) const {
    std::size_t result = 0;
    hashParam(result, renderPass.handle);
    return result;
  }
};

template <> struct hash<VkVertexInputAttributeDescription> {
  std::size_t
  operator()(const VkVertexInputAttributeDescription &attrib) const {
    std::size_t result = 0;

    hashParam(result, attrib.location);
    hashParam(result, attrib.binding);
    hashParam(result,
              static_cast<std::underlying_type<VkFormat>::type>(attrib.format));
    hashParam(result, attrib.offset);

    return result;
  }
};

template <> struct hash<VkVertexInputBindingDescription> {
  std::size_t operator()(const VkVertexInputBindingDescription &binding) const {
    std::size_t result = 0;

    hashParam(result, binding.binding);
    hashParam(result, binding.stride);
    hashParam(result,
              static_cast<std::underlying_type<VkVertexInputRate>::type>(
                  binding.inputRate));

    return result;
  }
};

template <> struct hash<PipelineState> {
  std::size_t operator()(const PipelineState &pipelineState) const {
    std::size_t result{0};

    hashParam(result, pipelineState.pipelineLayout->handle);

    if (auto renderPass = pipelineState.renderPass) {
      hashParam(result, renderPass->handle);
    }

    // hashParam(result, pipelineState.get_specialization_constant_state());

    hashParam(result, pipelineState.subpassIndex);

    // for (auto shader_module :
    //      pipelineState.get_pipeline_layout().get_shader_modules()) {
    //   hashParam(result, shader_module->get_id());
    // }

    for (const auto &attribute : pipelineState.vertexInputState.attributes) {
      hashParam(result, attribute);
    }

    for (auto &binding : pipelineState.vertexInputState.bindings) {
      hashParam(result, binding);
    }

    // VkPipelineInputAssemblyStateCreateInfo
    hashParam(result, pipelineState.inputAssemblyState.primitiveRestartEnable);
    hashParam(result,
              static_cast<std::underlying_type<VkPrimitiveTopology>::type>(
                  pipelineState.inputAssemblyState.topology));

    // VkPipelineViewportStateCreateInfo
    hashParam(result, pipelineState.viewportState.viewportCount);
    hashParam(result, pipelineState.viewportState.scissorCount);

    // VkPipelineRasterizationStateCreateInfo
    hashParam(result, pipelineState.rasterizationState.cullMode);
    hashParam(result, pipelineState.rasterizationState.depthBiasEnable);
    hashParam(result, pipelineState.rasterizationState.depthClampEnable);
    hashParam(result, static_cast<std::underlying_type<VkFrontFace>::type>(
                          pipelineState.rasterizationState.frontFace));
    hashParam(result, static_cast<std::underlying_type<VkPolygonMode>::type>(
                          pipelineState.rasterizationState.polygonMode));
    hashParam(result, pipelineState.rasterizationState.rasterizerDiscardEnable);

    // VkPipelineMultisampleStateCreateInfo
    hashParam(result, pipelineState.multisampleState.alphaToCoverageEnable);
    hashParam(result, pipelineState.multisampleState.alphaToOneEnable);
    hashParam(result, pipelineState.multisampleState.minSampleShading);
    hashParam(result,
              static_cast<std::underlying_type<VkSampleCountFlagBits>::type>(
                  pipelineState.multisampleState.rasterizationSamples));
    hashParam(result, pipelineState.multisampleState.sampleShadingEnable);
    hashParam(result, pipelineState.multisampleState.sampleMask);

    // VkPipelineDepthStencilStateCreateInfo
    hashParam(result, pipelineState.depthStencilState.back);
    hashParam(result, pipelineState.depthStencilState.depthBoundsTestEnable);
    hashParam(result, static_cast<std::underlying_type<VkCompareOp>::type>(
                          pipelineState.depthStencilState.depthCompareOp));
    hashParam(result, pipelineState.depthStencilState.depthTestEnable);
    hashParam(result, pipelineState.depthStencilState.depthWriteEnable);
    hashParam(result, pipelineState.depthStencilState.front);
    hashParam(result, pipelineState.depthStencilState.stencilTestEnable);

    // VkPipelineColorBlendStateCreateInfo
    hashParam(result, static_cast<std::underlying_type<VkLogicOp>::type>(
                          pipelineState.colorBlendState.logicOp));
    hashParam(result, pipelineState.colorBlendState.logicOpEnable);

    for (auto &attachment : pipelineState.colorBlendState.attachments) {
      hashParam(result, attachment);
    }

    return result;
  }
};

template <> struct hash<DescriptorSetLayout> {
  std::size_t operator()(const DescriptorSetLayout &descriptorSetLayout) const {
    std::size_t result = 0;
    hashParam(result, descriptorSetLayout.getHandle());
    return result;
  }
};

template <> struct hash<VkDescriptorBufferInfo> {
  std::size_t
  operator()(const VkDescriptorBufferInfo &descriptorBufferInfo) const {
    std::size_t result = 0;

    hashParam(result, descriptorBufferInfo.buffer);
    hashParam(result, descriptorBufferInfo.range);
    hashParam(result, descriptorBufferInfo.offset);

    return result;
  }
};

template <> struct hash<VkDescriptorImageInfo> {
  std::size_t
  operator()(const VkDescriptorImageInfo &descriptorImageInfo) const {
    std::size_t result = 0;

    hashParam(result, descriptorImageInfo.imageView);
    hashParam(result, static_cast<std::underlying_type<VkImageLayout>::type>(
                          descriptorImageInfo.imageLayout));
    hashParam(result, descriptorImageInfo.sampler);

    return result;
  }
};

template <> struct hash<VkWriteDescriptorSet> {
  std::size_t operator()(const VkWriteDescriptorSet &writeDescriptorSet) const {
    std::size_t result{0};

    hashParam(result, writeDescriptorSet.dstSet);
    hashParam(result, writeDescriptorSet.dstBinding);
    hashParam(result, writeDescriptorSet.dstArrayElement);
    hashParam(result, writeDescriptorSet.descriptorCount);
    hashParam(result, writeDescriptorSet.descriptorType);

    switch (writeDescriptorSet.descriptorType) {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i) {
        hashParam(result, writeDescriptorSet.pImageInfo[i]);
      }
      break;

    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i) {
        hashParam(result, writeDescriptorSet.pTexelBufferView[i]);
      }
      break;

    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i) {
        hashParam(result, writeDescriptorSet.pBufferInfo[i]);
      }
      break;

    default:
      break;
    };

    return result;
  }
};

template <> struct hash<DescriptorPool> {
  std::size_t operator()(const DescriptorPool &descriptorPool) const {
    std::size_t result{0};
    hashParam(result, descriptorPool.descriptorSetLayout);
    return result;
  }
};

template <> struct hash<StencilOpState> {
  std::size_t operator()(const StencilOpState &stencil) const {
    std::size_t result = 0;

    hashParam(result, static_cast<std::underlying_type<VkCompareOp>::type>(
                          stencil.compareOp));
    hashParam(result, static_cast<std::underlying_type<VkStencilOp>::type>(
                          stencil.depthFailOp));
    hashParam(result, static_cast<std::underlying_type<VkStencilOp>::type>(
                          stencil.failOp));
    hashParam(result, static_cast<std::underlying_type<VkStencilOp>::type>(
                          stencil.passOp));

    return result;
  }
};

template <> struct hash<ColorBlendAttachmentState> {
  std::size_t
  operator()(const ColorBlendAttachmentState &colorBlendAttachment) const {
    std::size_t result{0};

    hashParam(result, static_cast<std::underlying_type<VkBlendOp>::type>(
                          colorBlendAttachment.alphaBlendOp));
    hashParam(result, colorBlendAttachment.blendEnable);
    hashParam(result, static_cast<std::underlying_type<VkBlendOp>::type>(
                          colorBlendAttachment.colorBlendOp));
    hashParam(result, colorBlendAttachment.colorWriteMask);
    hashParam(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                          colorBlendAttachment.dstAlphaBlendFactor));
    hashParam(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                          colorBlendAttachment.dstColorBlendFactor));
    hashParam(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                          colorBlendAttachment.srcAlphaBlendFactor));
    hashParam(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                          colorBlendAttachment.srcColorBlendFactor));

    return result;
  }
};

} // namespace std

struct ResourceCacheState {
  std::unordered_map<std::size_t, std::unique_ptr<ShaderModule>> shaderModules;
  std::unordered_map<std::size_t, std::unique_ptr<Framebuffer>>  framebuffers;
  std::unordered_map<std::size_t, std::unique_ptr<RenderPass>>   renderPasses;
  std::unordered_map<std::size_t, std::unique_ptr<PipelineLayout>>
      pipelineLayouts;
  std::unordered_map<std::size_t, std::unique_ptr<DescriptorSetLayout>>
      descriptorSetLayouts;
  std::unordered_map<std::size_t, std::unique_ptr<GraphicsPipeline>>
      graphicsPipelines;
};

class ResourceCache {
public:
  ShaderModule *requestShaderModule(VkShaderStageFlagBits stage,
                                    const ShaderSource   &source,
                                    const ShaderVariant  &variant = {});
  RenderPass   *requestRenderPass(const std::vector<Attachment>  &attachments,
                                  const std::vector<LoadStoreOp> &loadStoreOps,
                                  const std::vector<SubpassInfo> &subpasses);
  Framebuffer  *requestFramebuffer(const RenderTarget &renderTarget,
                                   const RenderPass   &renderPass);
  PipelineLayout *
  requestPipelineLayout(const std::vector<ShaderModule *> &shaderModules);
  DescriptorSetLayout *requestDescriptorSetLayout(
      uint32_t setIndex, const std::vector<ShaderModule *> &shaderModules,
      const std::vector<ShaderResource> &shaderResources);
  GraphicsPipeline *requestGraphicsPipeline(const PipelineState &pipelineState);

  explicit ResourceCache(Device &device) : device{device} {}

  void clearFramebuffers();
  void clearPipelines();
  void clear();

private:
  Device            &device;
  ResourceCacheState state{};
  struct {
    std::mutex shaderModule;
    std::mutex renderPass;
    std::mutex framebuffer;
    std::mutex pipelineLayout;
    std::mutex descriptorSetLayout;
    std::mutex graphicsPipeline;
  } mutex;
};

template <typename T, typename... A,
          std::enable_if_t<std::is_base_of_v<Resource, T>, bool> = true>
T *requestResource(
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    Device                                              &device, A &...args) {
  std::size_t hash{0};
  hashParam(hash, args...);

  if (auto it = resources.find(hash); it != resources.end()) {
    return it->second.get();
  }

  auto resource = T::make(device, args...);
  if (!resource) { return nullptr; }

  auto it = resources.emplace(hash, std::move(resource));
  if (!it.second) { return nullptr; }

  return it.first->second.get();
}
