#include "renderer/pipeline.h"
#include "renderer/device.h"

Pipeline::~Pipeline() {
  vkDestroyPipeline(device.getHandle(), handle, nullptr);
}

std::unique_ptr<GraphicsPipeline>
GraphicsPipeline::make(Device &device, const PipelineState &pipelineState) {
  std::vector<VkShaderModule>                  shaderModules;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

  for (const auto shaderModule :
       pipelineState.getPipelineLayout()->getShaderModules()) {
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};

    shaderStageCreateInfo.stage = shaderModule->getStage();
    shaderStageCreateInfo.pName = shaderModule->getEntryPoint().c_str();

    // Create shader module
    VkShaderModuleCreateInfo shaderModuleCreateInfo{
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

    shaderModuleCreateInfo.codeSize =
        shaderModule->getSpirv().size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = shaderModule->getSpirv().data();

    if (vkCreateShaderModule(device.getHandle(), &shaderModuleCreateInfo,
                             nullptr,
                             &shaderStageCreateInfo.module) != VK_SUCCESS) {
      return nullptr;
    }

    shaderStageCreateInfos.push_back(shaderStageCreateInfo);
    shaderModules.push_back(shaderStageCreateInfo.module);
  }

  VkGraphicsPipelineCreateInfo createInfo{
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};

  createInfo.stageCount = shaderStageCreateInfos.size();
  createInfo.pStages    = shaderStageCreateInfos.data();

  VkPipelineVertexInputStateCreateInfo vertexInputState{
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

  vertexInputState.pVertexAttributeDescriptions =
      pipelineState.getVertexInputState().attributes.data();
  vertexInputState.vertexAttributeDescriptionCount =
      pipelineState.getVertexInputState().attributes.size();

  vertexInputState.pVertexBindingDescriptions =
      pipelineState.getVertexInputState().bindings.data();
  vertexInputState.vertexBindingDescriptionCount =
      pipelineState.getVertexInputState().bindings.size();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

  inputAssemblyState.topology = pipelineState.getInputAssemblyState().topology;
  inputAssemblyState.primitiveRestartEnable =
      pipelineState.getInputAssemblyState().primitiveRestartEnable;

  VkPipelineViewportStateCreateInfo viewportState{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};

  viewportState.viewportCount = pipelineState.viewportState.viewportCount;
  viewportState.scissorCount  = pipelineState.viewportState.scissorCount;

  VkPipelineRasterizationStateCreateInfo rasterizationState{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

  rasterizationState.depthClampEnable =
      pipelineState.rasterizationState.depthClampEnable;
  rasterizationState.rasterizerDiscardEnable =
      pipelineState.rasterizationState.rasterizerDiscardEnable;
  rasterizationState.polygonMode = pipelineState.rasterizationState.polygonMode;
  rasterizationState.cullMode    = pipelineState.rasterizationState.cullMode;
  rasterizationState.frontFace   = pipelineState.rasterizationState.frontFace;
  rasterizationState.depthBiasEnable =
      pipelineState.rasterizationState.depthBiasEnable;
  rasterizationState.depthBiasClamp       = 1.0f;
  rasterizationState.depthBiasSlopeFactor = 1.0f;
  rasterizationState.lineWidth            = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampleState{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

  multisampleState.sampleShadingEnable =
      pipelineState.multisampleState.sampleShadingEnable;
  multisampleState.rasterizationSamples =
      pipelineState.multisampleState.rasterizationSamples;
  multisampleState.minSampleShading =
      pipelineState.multisampleState.minSampleShading;
  multisampleState.alphaToCoverageEnable =
      pipelineState.multisampleState.alphaToCoverageEnable;
  multisampleState.alphaToOneEnable =
      pipelineState.multisampleState.alphaToOneEnable;

  if (pipelineState.multisampleState.sampleMask) {
    multisampleState.pSampleMask = &pipelineState.multisampleState.sampleMask;
  }

  VkPipelineDepthStencilStateCreateInfo depthStencilState{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

  depthStencilState.depthTestEnable =
      pipelineState.depthStencilState.depthTestEnable;
  depthStencilState.depthWriteEnable =
      pipelineState.depthStencilState.depthWriteEnable;
  depthStencilState.depthCompareOp =
      pipelineState.depthStencilState.depthCompareOp;
  depthStencilState.depthBoundsTestEnable =
      pipelineState.depthStencilState.depthBoundsTestEnable;
  depthStencilState.stencilTestEnable =
      pipelineState.depthStencilState.stencilTestEnable;
  depthStencilState.front.failOp = pipelineState.depthStencilState.front.failOp;
  depthStencilState.front.passOp = pipelineState.depthStencilState.front.passOp;
  depthStencilState.front.depthFailOp =
      pipelineState.depthStencilState.front.depthFailOp;
  depthStencilState.front.compareOp =
      pipelineState.depthStencilState.front.compareOp;
  depthStencilState.front.compareMask = ~0U;
  depthStencilState.front.writeMask   = ~0U;
  depthStencilState.front.reference   = ~0U;
  depthStencilState.back.failOp = pipelineState.depthStencilState.back.failOp;
  depthStencilState.back.passOp = pipelineState.depthStencilState.back.passOp;
  depthStencilState.back.depthFailOp =
      pipelineState.depthStencilState.back.depthFailOp;
  depthStencilState.back.compareOp =
      pipelineState.depthStencilState.back.compareOp;
  depthStencilState.back.compareMask = ~0U;
  depthStencilState.back.writeMask   = ~0U;
  depthStencilState.back.reference   = ~0U;

  VkPipelineColorBlendStateCreateInfo colorBlendState{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};

  colorBlendState.logicOpEnable = pipelineState.colorBlendState.logicOpEnable;
  colorBlendState.logicOp       = pipelineState.colorBlendState.logicOp;
  colorBlendState.attachmentCount =
      pipelineState.colorBlendState.attachments.size();
  colorBlendState.pAttachments =
      reinterpret_cast<const VkPipelineColorBlendAttachmentState *>(
          pipelineState.colorBlendState.attachments.data());
  colorBlendState.blendConstants[0] = 1.0f;
  colorBlendState.blendConstants[1] = 1.0f;
  colorBlendState.blendConstants[2] = 1.0f;
  colorBlendState.blendConstants[3] = 1.0f;

  std::array<VkDynamicState, 9> dynamicStates{
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
      VK_DYNAMIC_STATE_LINE_WIDTH,
      VK_DYNAMIC_STATE_DEPTH_BIAS,
      VK_DYNAMIC_STATE_BLEND_CONSTANTS,
      VK_DYNAMIC_STATE_DEPTH_BOUNDS,
      VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
      VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
      VK_DYNAMIC_STATE_STENCIL_REFERENCE,
  };

  VkPipelineDynamicStateCreateInfo dynamicState{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};

  dynamicState.pDynamicStates    = dynamicStates.data();
  dynamicState.dynamicStateCount = dynamicStates.size();

  createInfo.pVertexInputState   = &vertexInputState;
  createInfo.pInputAssemblyState = &inputAssemblyState;
  createInfo.pViewportState      = &viewportState;
  createInfo.pRasterizationState = &rasterizationState;
  createInfo.pMultisampleState   = &multisampleState;
  createInfo.pDepthStencilState  = &depthStencilState;
  createInfo.pColorBlendState    = &colorBlendState;
  createInfo.pDynamicState       = &dynamicState;

  createInfo.layout     = pipelineState.getPipelineLayout()->handle;
  createInfo.renderPass = pipelineState.renderPass->handle;
  createInfo.subpass    = pipelineState.subpassIndex;

  VkPipeline handle{VK_NULL_HANDLE};
  if (vkCreateGraphicsPipelines(device.getHandle(), nullptr, 1, &createInfo,
                                nullptr, &handle) != VK_SUCCESS) {
    return nullptr;
  }

  for (auto shaderModule : shaderModules) {
    vkDestroyShaderModule(device.getHandle(), shaderModule, nullptr);
  }

  auto pipeline    = std::make_unique<GraphicsPipeline>(device);
  pipeline->handle = handle;
  pipeline->state  = pipelineState;
  return pipeline;
}
