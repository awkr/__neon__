#pragma once

#include "renderer/image_view.h"
#include <functional>

struct RenderTarget {
  std::vector<Image> images;
  std::vector<ImageView> imageViews;
};

using RenderTargetCreateFunc =
    std::function<bool(Image color, RenderTarget *renderTarget)>;

extern const RenderTargetCreateFunc RENDER_TARGET_DEFAULT_CREATE_FUNC;

void destroyRenderTarget(RenderTarget *renderTarget);
