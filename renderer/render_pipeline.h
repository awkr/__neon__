#pragma once

#include "renderer/subpass.h"

struct RenderPipeline {
public:
  ~RenderPipeline();

  void addSubpass(std::unique_ptr<Subpass> &&subpass);
  bool draw(CommandBuffer &commandBuffer, RenderTarget &renderTarget);

private:
  std::vector<LoadStoreOp>  loadStoreOps = std::vector<LoadStoreOp>(2);
  std::vector<VkClearValue> clearValue{
      {.color = {0, 0, 0, 1}}, // Default clear color
      {.depthStencil = {0.0f, ~0U}},
  };

  std::vector<std::unique_ptr<Subpass>> subpasses;
};
