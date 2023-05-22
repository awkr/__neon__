#pragma once

#include "renderer/framebuffer.h"
#include "renderer/shader_module.h"
#include <glm/gtx/hash.hpp>
#include <mutex>

template <typename T> inline void hashCombine(size_t &seed, const T &v) {
  std::hash<T> hasher{};
  glm::detail::hash_combine(seed, hasher(v));
}

template <typename T> inline void hashParam(size_t &seed, const T &value) {
  hashCombine(seed, value);
}

template <typename T, typename... Args>
inline void hashParam(size_t &seed, const T &arg, const Args &...args) {
  hashParam(seed, arg);
  hashParam(seed, args...);
}

struct ResourceCacheState {
  std::unordered_map<std::size_t, std::unique_ptr<ShaderModule>> shaderModules;
  std::unordered_map<std::size_t, std::unique_ptr<Framebuffer>> framebuffers;
};

class ResourceCache {
public:
  ShaderModule *requestShaderModule(VkShaderStageFlagBits stage,
                                    const ShaderSource &source,
                                    const ShaderVariant &variant = {});

  void clearFramebuffers();
  void clear();

private:
  ResourceCacheState state{};
  struct {
    std::mutex shaderModule;
  } mutex;
};
