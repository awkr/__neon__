#pragma once

#include "renderer/subpass.h"

struct Mesh;

struct GeometrySubpass : public Subpass {
public:
  GeometrySubpass(RenderContext *renderContext, ShaderSource &&vertexShader,
                  ShaderSource &&fragmentShader);

  void prepare() override;

protected:
  std::vector<Mesh *> meshes;
};
