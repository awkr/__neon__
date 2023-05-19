#pragma once

#include "renderer/subpass.h"

struct RenderPipeline {
public:
  ~RenderPipeline();

  void addSubpass(std::unique_ptr<Subpass> &&subpass);

private:
  std::vector<VkClearValue> clearValue{
      {.color = {0, 0, 0, 1}},
      {.depthStencil = {0, ~0U}},
  };

  std::vector<std::unique_ptr<Subpass>> subpasses;
};
