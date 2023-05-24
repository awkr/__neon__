#include "renderer/subpass.h"

Subpass::Subpass(RenderContext *renderContext, ShaderSource &&vertexShader,
                 ShaderSource &&fragmentShader)
    : renderContext{renderContext}, vertexShader{std::move(vertexShader)},
      fragmentShader{std::move(fragmentShader)} {}

// void Subpass::updateRenderTarget(RenderTarget &renderTarget) {
//   renderTarget.setInputAttachments(inputAttachments);
//   renderTarget.setOutputAttachments(outputAttachments);
// }
