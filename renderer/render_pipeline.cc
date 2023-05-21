#include "renderer/render_pipeline.h"

RenderPipeline::~RenderPipeline() { subpasses.clear(); }

void RenderPipeline::addSubpass(std::unique_ptr<Subpass> &&subpass) {
  subpass->prepare();
  subpasses.emplace_back(std::move(subpass));
}

bool RenderPipeline::draw(CommandBuffer &commandBuffer,
                          RenderTarget &renderTarget) {
  while (clearValue.size() < renderTarget.getAttachments().size()) {
    clearValue.push_back({0.0f, 0.0f, 0.0f, 1.0f});
  }
  for (size_t i = 0; i < subpasses.size(); ++i) {
    auto &subpass = subpasses[i];
    // subpass->updateRenderTarget(renderTarget);
    if (i == 0) {
      if (!commandBuffer.beginRenderPass(renderTarget, loadStoreOps, clearValue,
                                         subpasses)) {
        return false;
      }
    } else {
      commandBuffer.nextSubpass();
    }
    subpass->draw();
  }
  commandBuffer.endRenderPass();
  return true;
}
