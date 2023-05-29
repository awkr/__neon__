#pragma once

#include "renderer/render_context.h"
#include "renderer/shader_module.h"

struct Subpass {
public:
  Subpass(RenderContext *renderContext, ShaderSource &&vertexShader,
          ShaderSource &&fragmentShader);

  virtual ~Subpass() = default;

  virtual void prepare()                          = 0;
  virtual void draw(CommandBuffer &commandBuffer) = 0;

  // void updateRenderTarget(RenderTarget &renderTarget);

  const std::vector<uint32_t> &getInputAttachments() const {
    return inputAttachments;
  }
  const std::vector<uint32_t> &getOutputAttachments() const {
    return outputAttachments;
  }

  bool enableDepthStencilAttachment{true};

protected:
  RenderContext *renderContext{nullptr};
  ShaderSource   vertexShader{};
  ShaderSource   fragmentShader{};

  // default to no input attachments
  std::vector<uint32_t> inputAttachments{};
  // default to swapchain output attachment
  std::vector<uint32_t> outputAttachments{0};
};
