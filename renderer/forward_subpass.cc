#include "renderer/forward_subpass.h"
#include "renderer/device.h"
#include "scene/mesh.h"

ForwardSubpass::ForwardSubpass(RenderContext *renderContext,
                               ShaderSource &&vertexShader,
                               ShaderSource &&fragmentShader)
    : GeometrySubpass(renderContext, std::move(vertexShader),
                      std::move(fragmentShader)) {}

void ForwardSubpass::prepare() {
  auto &cache = renderContext->getDevice()->getResourceCache();

  for (auto mesh : meshes) {
    for (auto subMesh : mesh->subMeshes) {
      auto &variant = subMesh->shaderVariant;

      cache.requestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, vertexShader,
                                variant);
      cache.requestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader,
                                variant);
    }
  }
}
