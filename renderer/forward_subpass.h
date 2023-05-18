#pragma once

#include "renderer/geometry_subpass.h"

struct ForwardSubpass : public GeometrySubpass {
public:
  ForwardSubpass(RenderContext *renderContext, ShaderSource &&vertexShader,
                 ShaderSource &&fragmentShader);

  void prepare() override;
};
