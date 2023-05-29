#include "renderer/render_pipeline.h"

RenderPipeline::~RenderPipeline() { subpasses.clear(); }

void RenderPipeline::addSubpass(std::unique_ptr<Subpass> &&subpass) {
  subpass->prepare();
  subpasses.emplace_back(std::move(subpass));
}

bool RenderPipeline::draw(CommandBuffer &commandBuffer,
                          RenderTarget  &renderTarget) {
  while (clearValue.size() < renderTarget.getAttachments().size()) {
    clearValue.push_back(clearValue.front());
  }

  if (!commandBuffer.beginRenderPass(renderTarget, loadStoreOps, clearValue,
                                     subpasses)) {
    return false;
  }

  for (size_t i = 0; i < subpasses.size(); ++i) {
    if (i > 0) { commandBuffer.nextSubpass(); }
    subpasses[i]->draw(commandBuffer);
  }

  commandBuffer.endRenderPass();

  return true;
}
