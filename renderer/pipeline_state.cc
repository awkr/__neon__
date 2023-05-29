#include "renderer/pipeline_state.h"
#include "renderer/render_pass.h"
#include <tuple>

bool operator!=(const RasterizationState &lhs, const RasterizationState &rhs) {
  return std::tie(lhs.cullMode, lhs.depthBiasEnable, lhs.depthClampEnable,
                  lhs.frontFace, lhs.frontFace, lhs.polygonMode,
                  lhs.rasterizerDiscardEnable) !=
         std::tie(rhs.cullMode, rhs.depthBiasEnable, rhs.depthClampEnable,
                  rhs.frontFace, rhs.frontFace, rhs.polygonMode,
                  rhs.rasterizerDiscardEnable);
}

bool operator!=(const MultisampleState &lhs, const MultisampleState &rhs) {
  return std::tie(lhs.alphaToCoverageEnable, lhs.alphaToOneEnable,
                  lhs.minSampleShading, lhs.rasterizationSamples,
                  lhs.sampleMask, lhs.sampleShadingEnable) !=
         std::tie(rhs.alphaToCoverageEnable, rhs.alphaToOneEnable,
                  rhs.minSampleShading, rhs.rasterizationSamples,
                  rhs.sampleMask, rhs.sampleShadingEnable);
}

bool operator==(const VkVertexInputAttributeDescription &lhs,
                const VkVertexInputAttributeDescription &rhs) {
  return std::tie(lhs.binding, lhs.format, lhs.location, lhs.offset) ==
         std::tie(rhs.binding, rhs.format, rhs.location, rhs.offset);
}

bool operator==(const VkVertexInputBindingDescription &lhs,
                const VkVertexInputBindingDescription &rhs) {
  return std::tie(lhs.binding, lhs.inputRate, lhs.stride) ==
         std::tie(rhs.binding, rhs.inputRate, rhs.stride);
}

bool operator==(const ColorBlendAttachmentState &lhs,
                const ColorBlendAttachmentState &rhs) {
  return std::tie(lhs.alphaBlendOp, lhs.blendEnable, lhs.colorBlendOp,
                  lhs.colorWriteMask, lhs.dstAlphaBlendFactor,
                  lhs.dstColorBlendFactor, lhs.srcAlphaBlendFactor,
                  lhs.srcColorBlendFactor) ==
         std::tie(rhs.alphaBlendOp, rhs.blendEnable, rhs.colorBlendOp,
                  rhs.colorWriteMask, rhs.dstAlphaBlendFactor,
                  rhs.dstColorBlendFactor, rhs.srcAlphaBlendFactor,
                  rhs.srcColorBlendFactor);
}

bool operator!=(const ColorBlendState &lhs, const ColorBlendState &rhs) {
  return std::tie(lhs.logicOp, lhs.logicOpEnable) !=
             std::tie(rhs.logicOp, rhs.logicOpEnable) ||
         lhs.attachments.size() != rhs.attachments.size() ||
         !std::equal(
             lhs.attachments.begin(), lhs.attachments.end(),
             rhs.attachments.begin(),
             [](const ColorBlendAttachmentState &lhs,
                const ColorBlendAttachmentState &rhs) { return lhs == rhs; });
}

bool operator!=(const VertexInputState &lhs, const VertexInputState &rhs) {
  return lhs.attributes != rhs.attributes || lhs.bindings != rhs.bindings;
}

void PipelineState::setRasterizationState(const RasterizationState &in) {
  if (in != rasterizationState) {
    rasterizationState = in;
    dirty              = true;
  }
}

void PipelineState::setMultisampleState(const MultisampleState &in) {
  if (in != multisampleState) {
    multisampleState = in;
    dirty            = true;
  }
}

void PipelineState::setVertexInputState(const VertexInputState &in) {
  if (in != vertexInputState) {
    vertexInputState = in;
    dirty            = true;
  }
}

void PipelineState::setColorBlendState(const ColorBlendState &in) {
  if (colorBlendState != in) {
    colorBlendState = in;
    dirty           = true;
  }
}

void PipelineState::setPipelineLayout(const PipelineLayout &in) {
  if (pipelineLayout) {
    if (pipelineLayout->handle != in.handle) {
      pipelineLayout = &in;
      dirty          = true;
    }
  } else {
    pipelineLayout = &in;
    dirty          = true;
  }
}

void PipelineState::setRenderPass(const RenderPass &in) {
  if (renderPass) {
    if (renderPass->handle != in.handle) {
      renderPass = &in;
      dirty      = true;
    }
  } else {
    renderPass = &in;
    dirty      = true;
  }
}

void PipelineState::setSubpassIndex(uint32_t index) {
  if (subpassIndex != index) {
    subpassIndex = index;
    dirty        = true;
  }
}

void PipelineState::reset() {
  clearDirty();
  pipelineLayout     = nullptr;
  renderPass         = nullptr;
  rasterizationState = {};
  multisampleState   = {};
  vertexInputState   = {};
  inputAssemblyState = {};
  viewportState      = {};
  depthStencilState  = {};
  colorBlendState    = {};
  subpassIndex       = 0;
}

void PipelineState::clearDirty() { dirty = false; }
