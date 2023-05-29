#pragma once

#include "renderer/subpass.h"

struct Mesh;
struct SubMesh;
struct Scene;

struct GeometrySubpass : public Subpass {
public:
  GeometrySubpass(RenderContext *renderContext, ShaderSource &&vertexShader,
                  ShaderSource &&fragmentShader, Scene &scene);

  void prepare() override;
  void draw(CommandBuffer &commandBuffer) override;

protected:
  virtual void updateUniform(CommandBuffer &commandBuffer);
  bool         drawSubMesh(CommandBuffer &commandBuffer, const SubMesh &subMesh,
                           VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);
  bool drawSubMeshCommand(CommandBuffer &commandBuffer, const SubMesh &subMesh);
  virtual void setPipelineState(CommandBuffer &commandBuffer,
                                VkFrontFace frontFace, bool doubleSided);
  PipelineLayout *
  preparePipelineLayout(CommandBuffer                     &commandBuffer,
                        const std::vector<ShaderModule *> &shaderModules);

  std::vector<Mesh *>   meshes;
  RasterizationState    baseRasterizationState{};
  VkSampleCountFlagBits sampleCount{VK_SAMPLE_COUNT_1_BIT};
};
