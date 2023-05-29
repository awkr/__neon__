#pragma once

#include "renderer/pipeline_layout.h"

struct RenderPass;

struct RasterizationState {
  VkBool32        depthClampEnable{VK_FALSE};
  VkBool32        rasterizerDiscardEnable{VK_FALSE};
  VkPolygonMode   polygonMode{VK_POLYGON_MODE_FILL};
  VkCullModeFlags cullMode{VK_CULL_MODE_BACK_BIT};
  VkFrontFace     frontFace{VK_FRONT_FACE_COUNTER_CLOCKWISE};
  VkBool32        depthBiasEnable{VK_FALSE};
};

struct MultisampleState {
  VkSampleCountFlagBits rasterizationSamples{VK_SAMPLE_COUNT_1_BIT};
  VkBool32              sampleShadingEnable{VK_FALSE};
  float                 minSampleShading{0.0f};
  VkSampleMask          sampleMask{0};
  VkBool32              alphaToCoverageEnable{VK_FALSE};
  VkBool32              alphaToOneEnable{VK_FALSE};
};

struct VertexInputState {
  std::vector<VkVertexInputBindingDescription>   bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;
};

struct InputAssemblyState {
  VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
  VkBool32            primitiveRestartEnable{VK_FALSE};
};

struct ViewportState {
  uint32_t viewportCount{1};
  uint32_t scissorCount{1};
};

struct StencilOpState {
  VkStencilOp failOp{VK_STENCIL_OP_REPLACE};
  VkStencilOp passOp{VK_STENCIL_OP_REPLACE};
  VkStencilOp depthFailOp{VK_STENCIL_OP_REPLACE};
  VkCompareOp compareOp{VK_COMPARE_OP_NEVER};
};

struct DepthStencilState {
  VkBool32 depthTestEnable{VK_TRUE};
  VkBool32 depthWriteEnable{VK_TRUE};
  // Using Reversed depth-buffer for increased precision, so greater depth
  // values are kept
  VkCompareOp    depthCompareOp{VK_COMPARE_OP_GREATER};
  VkBool32       depthBoundsTestEnable{VK_FALSE};
  VkBool32       stencilTestEnable{VK_FALSE};
  StencilOpState front{};
  StencilOpState back{};
};

struct ColorBlendAttachmentState {
  VkBool32              blendEnable{VK_FALSE};
  VkBlendFactor         srcColorBlendFactor{VK_BLEND_FACTOR_ONE};
  VkBlendFactor         dstColorBlendFactor{VK_BLEND_FACTOR_ZERO};
  VkBlendOp             colorBlendOp{VK_BLEND_OP_ADD};
  VkBlendFactor         srcAlphaBlendFactor{VK_BLEND_FACTOR_ONE};
  VkBlendFactor         dstAlphaBlendFactor{VK_BLEND_FACTOR_ZERO};
  VkBlendOp             alphaBlendOp{VK_BLEND_OP_ADD};
  VkColorComponentFlags colorWriteMask{
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
};

struct ColorBlendState {
  VkBool32                               logicOpEnable{VK_FALSE};
  VkLogicOp                              logicOp{VK_LOGIC_OP_CLEAR};
  std::vector<ColorBlendAttachmentState> attachments;
};

struct PipelineState {
  void setRasterizationState(const RasterizationState &in);
  void setMultisampleState(const MultisampleState &in);
  void setVertexInputState(const VertexInputState &in);
  void setColorBlendState(const ColorBlendState &in);
  void setPipelineLayout(const PipelineLayout &in);
  void setRenderPass(const RenderPass &in);
  void setSubpassIndex(uint32_t index);

  void reset();
  void clearDirty();

  const PipelineLayout  *getPipelineLayout() const { return pipelineLayout; }
  const ColorBlendState &getColorBlendState() const { return colorBlendState; }

  const VertexInputState &getVertexInputState() const {
    return vertexInputState;
  }

  const InputAssemblyState &getInputAssemblyState() const {
    return inputAssemblyState;
  }

  bool isDirty() const { return dirty; }

  const PipelineLayout *pipelineLayout{nullptr};
  RasterizationState    rasterizationState{};
  MultisampleState      multisampleState{};
  VertexInputState      vertexInputState{};
  InputAssemblyState    inputAssemblyState{};
  ViewportState         viewportState{};
  DepthStencilState     depthStencilState{};
  ColorBlendState       colorBlendState{};
  const RenderPass     *renderPass{nullptr};
  uint32_t              subpassIndex{0U};
  bool                  dirty{false};
};
