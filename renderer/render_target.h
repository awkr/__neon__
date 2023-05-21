#pragma once

#include "renderer/image_view.h"
#include <functional>

struct RenderTarget {
  using CreateFunc = std::function<std::unique_ptr<RenderTarget>(Image color)>;

  static const CreateFunc DEFAULT_CREATE_FUNC;

  ~RenderTarget();

  std::vector<Image> images;
  std::vector<ImageView> imageViews;
};
