#pragma once

#include "renderer/render_target.h"

struct RenderFrame {
  RenderTarget renderTarget{};
};

void destroyRenderFrame(RenderFrame *frame);
