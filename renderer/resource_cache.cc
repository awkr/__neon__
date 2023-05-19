#include "renderer/resource_cache.h"
#include "renderer/shader_module.h"

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

} // namespace std

template <typename T, typename... A,
          std::enable_if_t<std::is_base_of_v<Resource, T>, bool> = true>
T *requestResource(
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    A &...args) {
  std::size_t hash{0U};
  hashParam(hash, args...);

  if (auto it = resources.find(hash); it != resources.end()) {
    return it->second.get();
  }

  auto resource = T::make(args...);
  if (!resource) { return nullptr; }

  auto it = resources.emplace(hash, std::move(resource));
  if (!it.second) { return nullptr; }

  return it.first->second.get();
}

template <typename T, typename... A>
T *requestResource(
    std::mutex &mutex,
    std::unordered_map<std::size_t, std::unique_ptr<T>> &resources,
    A &...args) {
  std::lock_guard<std::mutex> guard(mutex);
  return requestResource(resources, args...);
}

ShaderModule *ResourceCache::requestShaderModule(VkShaderStageFlagBits stage,
                                                 const ShaderSource &source,
                                                 const ShaderVariant &variant) {
  std::string entry{"main"};
  return requestResource(mutex.shaderModule, state.shaderModules, stage, source,
                         entry, variant);
}
