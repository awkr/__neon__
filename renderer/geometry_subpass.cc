#include "renderer/geometry_subpass.h"
#include "renderer/device.h"
#include "scene/material.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "scene/sub_mesh.h"

GeometrySubpass::GeometrySubpass(RenderContext *renderContext,
                                 ShaderSource &&vertexShader,
                                 ShaderSource &&fragmentShader, Scene &scene)
    : Subpass(renderContext, std::move(vertexShader),
              std::move(fragmentShader)) {}

void GeometrySubpass::prepare() {
  // TODO
}

void GeometrySubpass::draw(CommandBuffer &commandBuffer) {
  std::vector<SubMesh *> subMeshes;
  for (const auto mesh : meshes) {
    for (const auto subMesh : mesh->subMeshes) {
      subMeshes.emplace_back(subMesh);
    }
  }

  for (const auto subMesh : subMeshes) {
    updateUniform(commandBuffer);
    drawSubMesh(commandBuffer, *subMesh);
  }

  // TODO
}

void GeometrySubpass::updateUniform(CommandBuffer &commandBuffer) {}

bool GeometrySubpass::drawSubMesh(CommandBuffer &commandBuffer,
                                  const SubMesh &subMesh,
                                  VkFrontFace    frontFace) {
  auto &device = commandBuffer.device;

  setPipelineState(commandBuffer, frontFace, subMesh.material->doubleSided);

  auto vertShaderModule = device.getResourceCache().requestShaderModule(
      VK_SHADER_STAGE_VERTEX_BIT, vertexShader, subMesh.shaderVariant);
  auto fragShaderModule = device.getResourceCache().requestShaderModule(
      VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader, subMesh.shaderVariant);

  std::vector<ShaderModule *> shaderModules{vertShaderModule, fragShaderModule};
  auto pipelineLayout = preparePipelineLayout(commandBuffer, shaderModules);
  if (!pipelineLayout) { return false; }
  commandBuffer.bindPipelineLayout(*pipelineLayout);

  // TODO

  auto vertexInputResources = pipelineLayout->getShaderResources(
      ShaderResourceType::Input, VK_SHADER_STAGE_VERTEX_BIT);

  VertexInputState vertexInputState{};

  for (const auto resource : vertexInputResources) {
    VertexAttribute attribute{};
    if (!subMesh.getAttribute(resource->name, attribute)) { continue; }

    VkVertexInputAttributeDescription vertexAttribute{
        .location = resource->location,
        .binding  = resource->binding,
        .format   = attribute.format,
        .offset   = attribute.offset};

    vertexInputState.attributes.push_back(vertexAttribute);

    VkVertexInputBindingDescription vertexBinding{.binding = resource->binding,
                                                  .stride  = attribute.stride};

    vertexInputState.bindings.push_back(vertexBinding);
  }

  commandBuffer.setVertexInputState(vertexInputState);

  // find submesh vertex buffers matching the shader input attribute name
  for (const auto resource : vertexInputResources) {
    auto it = subMesh.vertexBuffers.find(resource->name);
    if (it != subMesh.vertexBuffers.end()) {
      std::vector<std::reference_wrapper<const Buffer>> buffers;
      buffers.emplace_back(std::ref(it->second));

      commandBuffer.bindVertexBuffers(resource->location, buffers,
                                      {resource->offset});
    }
  }

  return drawSubMeshCommand(commandBuffer, subMesh);
}

bool GeometrySubpass::drawSubMeshCommand(CommandBuffer &commandBuffer,
                                         const SubMesh &subMesh) {
  if (subMesh.indexCount > 0) { // TODO
    commandBuffer.bindIndexBuffer();
    commandBuffer.drawIndexed();
  } else {
    commandBuffer.draw(subMesh.vertexCount, 1, 0, 0);
  }
  return true;
}

void GeometrySubpass::setPipelineState(CommandBuffer &commandBuffer,
                                       VkFrontFace    frontFace,
                                       bool           doubleSided) {
  auto rasterizationState      = baseRasterizationState;
  rasterizationState.frontFace = frontFace;
  if (doubleSided) { rasterizationState.cullMode = VK_CULL_MODE_NONE; }

  commandBuffer.setRasterizationState(rasterizationState);

  MultisampleState multisampleState{};
  multisampleState.rasterizationSamples = sampleCount;

  commandBuffer.setMultisampleState(multisampleState);
}

PipelineLayout *GeometrySubpass::preparePipelineLayout(
    CommandBuffer                     &commandBuffer,
    const std::vector<ShaderModule *> &shaderModules) {
  return commandBuffer.device.getResourceCache().requestPipelineLayout(
      shaderModules);
}
