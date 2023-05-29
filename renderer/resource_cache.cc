#include "renderer/resource_cache.h"

template <typename T, typename... A>
T *requestResource(
    std::mutex                                          &mutex,
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    Device                                              &device, A &...args) {
  std::lock_guard<std::mutex> guard(mutex);
  return requestResource(resources, device, args...);
}

ShaderModule *ResourceCache::requestShaderModule(VkShaderStageFlagBits stage,
                                                 const ShaderSource   &source,
                                                 const ShaderVariant &variant) {
  std::string entry{"main"};
  return requestResource(mutex.shaderModule, state.shaderModules, device, stage,
                         source, entry, variant);
}

RenderPass *
ResourceCache::requestRenderPass(const std::vector<Attachment>  &attachments,
                                 const std::vector<LoadStoreOp> &loadStoreOps,
                                 const std::vector<SubpassInfo> &subpasses) {
  return requestResource(mutex.renderPass, state.renderPasses, device,
                         attachments, loadStoreOps, subpasses);
}

Framebuffer *ResourceCache::requestFramebuffer(const RenderTarget &renderTarget,
                                               const RenderPass   &renderPass) {
  return requestResource(mutex.framebuffer, state.framebuffers, device,
                         renderTarget, renderPass);
}

PipelineLayout *ResourceCache::requestPipelineLayout(
    const std::vector<ShaderModule *> &shaderModules) {
  return requestResource(mutex.pipelineLayout, state.pipelineLayouts, device,
                         shaderModules);
}

DescriptorSetLayout *ResourceCache::requestDescriptorSetLayout(
    uint32_t setIndex, const std::vector<ShaderModule *> &shaderModules,
    const std::vector<ShaderResource> &shaderResources) {
  return requestResource(mutex.descriptorSetLayout, state.descriptorSetLayouts,
                         device, setIndex, shaderModules, shaderResources);
}

GraphicsPipeline *
ResourceCache::requestGraphicsPipeline(const PipelineState &pipelineState) {
  return requestResource(mutex.graphicsPipeline, state.graphicsPipelines,
                         device, pipelineState);
}

void ResourceCache::clearFramebuffers() { state.framebuffers.clear(); }

void ResourceCache::clearPipelines() { state.graphicsPipelines.clear(); }

void ResourceCache::clear() {
  clearPipelines();
  state.descriptorSetLayouts.clear();
  state.pipelineLayouts.clear();
  state.renderPasses.clear();
  clearFramebuffers();
  state.shaderModules.clear();
}
