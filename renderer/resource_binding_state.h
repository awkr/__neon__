#pragma once

#include "renderer/buffer.h"
#include "renderer/image_view.h"
#include "renderer/sampler.h"
#include <map>
#include <unordered_map>

template <class T> using BindingMap = std::map<uint32_t, std::map<uint32_t, T>>;

/**
 * @brief A resource info is a struct containing the actual resource data.
 *
 * This will be referenced by a buffer info or image info descriptor inside a
 * descriptor set.
 */
struct ResourceInfo {
  const Buffer    *buffer{nullptr};
  VkDeviceSize     offset{0};
  VkDeviceSize     range{0};
  const ImageView *imageView{nullptr};
  const Sampler   *sampler{nullptr};
  bool             dirty{false};
};

/**
 * @brief A resource set is a set of bindings containing resources that were
 * bound by a command buffer.
 *
 * The ResourceSet has a one to one mapping with a DescriptorSet.
 */
struct ResourceSet {
  void clearDirty() { dirty = false; }

  const BindingMap<ResourceInfo> &getResourceBindings() const {
    return resourceBindings;
  }

  BindingMap<ResourceInfo> resourceBindings;
  bool                     dirty{false};
};

/**
 * @brief The resource binding state of a command buffer.
 *
 * Keeps track of all the resources bound by the command buffer. The
 * ResourceBindingState is used by the command buffer to create the appropriate
 * descriptor sets when it comes to draw.
 */
struct ResourceBindingState {
  void reset();
  void clearDirty();

  std::unordered_map<uint32_t, ResourceSet> resourceSets;
  bool                                      dirty{false};
};
