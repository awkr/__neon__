#include "renderer/geometry_subpass.h"

GeometrySubpass::GeometrySubpass(RenderContext *renderContext,
                                 ShaderSource &&vertexShader,
                                 ShaderSource &&fragmentShader)
    : Subpass(renderContext, std::move(vertexShader),
              std::move(fragmentShader)) {}

void GeometrySubpass::prepare() {}

void GeometrySubpass::draw() {}
