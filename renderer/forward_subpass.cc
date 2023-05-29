#include "renderer/forward_subpass.h"
#include "renderer/device.h"
#include "scene/mesh.h"

ForwardSubpass::ForwardSubpass(RenderContext *renderContext,
                               ShaderSource &&vertexShader,
                               ShaderSource &&fragmentShader, Scene &scene)
    : GeometrySubpass(renderContext, std::move(vertexShader),
                      std::move(fragmentShader), scene) {}

void ForwardSubpass::prepare() {
  auto &cache = renderContext->getDevice()->getResourceCache();

  for (const auto &mesh : meshes) {
    for (const auto &subMesh : mesh->subMeshes) {
      const auto &variant = subMesh->shaderVariant;

      cache.requestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, vertexShader,
                                variant);
      cache.requestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader,
                                variant);
    }
  }
}

void ForwardSubpass::draw(CommandBuffer &commandBuffer) {
  GeometrySubpass::draw(commandBuffer);
}
