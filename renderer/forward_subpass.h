#pragma once

#include "renderer/geometry_subpass.h"

struct ForwardSubpass : public GeometrySubpass {
public:
  ForwardSubpass(RenderContext *renderContext, ShaderSource &&vertexShader,
                 ShaderSource &&fragmentShader, Scene &scene);

  void prepare() override;
  void draw(CommandBuffer &commandBuffer) override;
};
