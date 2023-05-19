#include "renderer/render_pipeline.h"

RenderPipeline::~RenderPipeline() { subpasses.clear(); }

void RenderPipeline::addSubpass(std::unique_ptr<Subpass> &&subpass) {
  subpass->prepare();
  subpasses.emplace_back(std::move(subpass));
}
