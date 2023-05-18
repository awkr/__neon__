#pragma once

#include "renderer/shader_module.h"
#include "scene/component.h"

struct SubMesh : public Component {
  ShaderVariant shaderVariant{};
};
