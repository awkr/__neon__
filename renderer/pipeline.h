#pragma once

#include "renderer/pipeline_state.h"
#include "renderer/resource.h"

struct Pipeline : public Resource {
  explicit Pipeline(Device &device) : device{device} {}
  ~Pipeline() override;

  VkPipeline getHandle() const { return handle; }

  Device       &device;
  VkPipeline    handle{VK_NULL_HANDLE};
  PipelineState state{};
};

struct GraphicsPipeline : public Pipeline {
  static std::unique_ptr<GraphicsPipeline>
  make(Device &device, const PipelineState &pipelineState);

  explicit GraphicsPipeline(Device &device) : Pipeline(device) {}
};
