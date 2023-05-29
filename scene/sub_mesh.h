#pragma once

#include "renderer/buffer.h"
#include "renderer/shader_module.h"
#include "scene/component.h"

struct Material;

struct VertexAttribute {
  VkFormat format{VK_FORMAT_UNDEFINED};
  uint32_t stride{0};
  uint32_t offset{0};
};

struct SubMesh : public Component {
  bool getAttribute(const std::string &name, VertexAttribute &attribute) const;

  std::unordered_map<std::string, VertexAttribute> vertexAttributes;
  std::unordered_map<std::string, Buffer>          vertexBuffers;
  uint32_t                                         vertexCount{0};
  uint32_t                                         indexCount{0};
  ShaderVariant                                    shaderVariant{};
  std::unique_ptr<Material>                        material;
};
