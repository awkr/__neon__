#pragma once

#include "renderer/render_context.h"
#include "renderer/shader_module.h"

struct Subpass {
public:
  Subpass(RenderContext *renderContext, ShaderSource &&vertexShader,
          ShaderSource &&fragmentShader);

  virtual ~Subpass() = default;

  virtual void prepare() = 0;

protected:
  RenderContext *renderContext{nullptr};
  ShaderSource vertexShader{};
  ShaderSource fragmentShader{};
};
