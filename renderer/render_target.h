#pragma once

#include "renderer/image.h"
#include <functional>

struct RenderTarget {
  std::vector<Image> images;
};

using RenderTargetCreateFunc = std::function<RenderTarget *(Image image)>;

extern const RenderTargetCreateFunc RENDER_TARGET_DEFAULT_CREATE_FUNC;

void destroyRenderTarget(RenderTarget *renderTarget);
