#include "renderer/render_pipeline.h"

RenderPipeline::~RenderPipeline() {
  for (auto subpass : subpasses) {
    delete subpass;
  }
  subpasses.clear();
}

void RenderPipeline::addSubpass(Subpass *subpass) {
  subpass->prepare();
  subpasses.emplace_back(subpass);
}
