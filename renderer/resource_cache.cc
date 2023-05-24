#include "renderer/resource_cache.h"
#include "renderer/render_target.h"

namespace std {

template <> struct hash<ShaderSource> {
  std::size_t operator()(const ShaderSource &source) const {
    std::size_t result{0};
    hashCombine(result, source.id);
    return result;
  }
};

template <> struct hash<ShaderVariant> {
  std::size_t operator()(const ShaderVariant &variant) const {
    std::size_t result{0};
    hashCombine(result, variant.getId());
    return result;
  }
};

template <> struct hash<Attachment> {
  std::size_t operator()(const Attachment &attachment) const {
    std::size_t result = 0;

    hashCombine(result, static_cast<std::underlying_type<VkFormat>::type>(
                            attachment.format));
    hashCombine(result,
                static_cast<std::underlying_type<VkSampleCountFlagBits>::type>(
                    attachment.sampleCount));
    hashCombine(result, attachment.usage);
    hashCombine(result, static_cast<std::underlying_type<VkImageLayout>::type>(
                            attachment.initialLayout));

    return result;
  }
};

template <> struct hash<LoadStoreOp> {
  std::size_t operator()(const LoadStoreOp &load_store_info) const {
    std::size_t result = 0;

    hashCombine(result,
                static_cast<std::underlying_type<VkAttachmentLoadOp>::type>(
                    load_store_info.loadOp));
    hashCombine(result,
                static_cast<std::underlying_type<VkAttachmentStoreOp>::type>(
                    load_store_info.storeOp));

    return result;
  }
};

template <> struct hash<SubpassInfo> {
  std::size_t operator()(const SubpassInfo &subpassInfo) const {
    std::size_t result = 0;

    for (auto output : subpassInfo.outputAttachments) {
      hashCombine(result, output);
    }
    for (auto input : subpassInfo.inputAttachments) {
      hashCombine(result, input);
    }
    hashCombine(result, subpassInfo.enableDepthStencilAttachment);

    return result;
  }
};

template <> struct hash<RenderTarget> {
  std::size_t operator()(const RenderTarget &renderTarget) const {
    std::size_t result = 0;
    for (const auto &imageView : renderTarget.imageViews) {
      hashCombine(result, imageView.handle);
      hashCombine(result, imageView.image->handle);
    }
    return result;
  }
};

template <> struct hash<RenderPass> {
  std::size_t operator()(const RenderPass &renderPass) const {
    std::size_t result = 0;
    hashCombine(result, renderPass.handle);
    return result;
  }
};

} // namespace std

template <>
inline void
hashParam<std::vector<Attachment>>(size_t &seed,
                                   const std::vector<Attachment> &value) {
  for (const auto &attachment : value) { hashCombine(seed, attachment); }
}

template <>
inline void
hashParam<std::vector<LoadStoreOp>>(size_t &seed,
                                    const std::vector<LoadStoreOp> &value) {
  for (const auto &loadStoreInfo : value) { hashCombine(seed, loadStoreInfo); }
}

template <>
inline void
hashParam<std::vector<SubpassInfo>>(size_t &seed,
                                    const std::vector<SubpassInfo> &value) {
  for (const auto &subpassInfo : value) { hashCombine(seed, subpassInfo); }
}

template <typename T, typename... A,
          std::enable_if_t<std::is_base_of_v<Resource, T>, bool> = true>
T *requestResource(
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    Device &device, A &...args) {
  std::size_t hash{0U};
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

template <typename T, typename... A>
T *requestResource(
    std::mutex &mutex,
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    Device &device, A &...args) {
  std::lock_guard<std::mutex> guard(mutex);
  return requestResource(resources, device, args...);
}

ShaderModule *ResourceCache::requestShaderModule(Device &device,
                                                 VkShaderStageFlagBits stage,
                                                 const ShaderSource &source,
                                                 const ShaderVariant &variant) {
  std::string entry{"main"};
  return requestResource(mutex.shaderModule, state.shaderModules, device, stage,
                         source, entry, variant);
}

RenderPass *
ResourceCache::requestRenderPass(Device &device,
                                 const std::vector<Attachment> &attachments,
                                 const std::vector<LoadStoreOp> &loadStoreOps,
                                 const std::vector<SubpassInfo> &subpasses) {
  return requestResource(mutex.renderPass, state.renderPasses, device,
                         attachments, loadStoreOps, subpasses);
}

Framebuffer *ResourceCache::requestFramebuffer(Device &device,
                                               const RenderTarget &renderTarget,
                                               const RenderPass &renderPass) {
  return requestResource(mutex.framebuffer, state.framebuffers, device,
                         renderTarget, renderPass);
}

void ResourceCache::clearFramebuffers() { state.framebuffers.clear(); }

void ResourceCache::clear() {
  state.renderPasses.clear();
  clearFramebuffers();
  state.shaderModules.clear();
}
