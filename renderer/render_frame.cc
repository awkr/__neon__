#include "renderer/render_frame.h"

void destroyRenderFrame(RenderFrame *frame) {
  destroyRenderTarget(&frame->renderTarget);
}
