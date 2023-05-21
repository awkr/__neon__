#include "renderer/device.h"
#include "renderer/forward_subpass.h"
#include "renderer/instance.h"
#include "renderer/render_context.h"
#include "renderer/render_pipeline.h"
#include <GLFW/glfw3.h>
#include <cstdio>

const char *windowTitle = "neon";
uint32_t windowWidth{800};
uint32_t windowHeight{500};

VkInstance instance{VK_NULL_HANDLE};
VkDebugUtilsMessengerEXT messenger{VK_NULL_HANDLE};
VkSurfaceKHR surface{VK_NULL_HANDLE};
Device device{};

std::unique_ptr<RenderContext> renderContext;

void setViewport(CommandBuffer &commandBuffer, const VkExtent2D &extent) {
  VkViewport viewport{};
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  commandBuffer.setViewport(viewport);
}

void setScissor(CommandBuffer &commandBuffer, const VkExtent2D &extent) {
  VkRect2D scissor{.extent = extent};
  commandBuffer.setScissor(scissor);
}

void draw(CommandBuffer &commandBuffer, RenderTarget *renderTarget) {
  setViewport(commandBuffer, renderTarget->extent);
  setScissor(commandBuffer, renderTarget->extent);

  // TODO
}

void render(CommandBuffer &commandBuffer, RenderTarget *renderTarget) {
  auto &imageViews = renderTarget->imageViews;
  { // image 0 is the swapchain
    ImageMemoryBarrier memoryBarrier{};
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    memoryBarrier.srcAccess = 0;
    memoryBarrier.dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    memoryBarrier.srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    memoryBarrier.dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    commandBuffer.imageMemoryBarrier(imageViews[0], memoryBarrier);

    for (size_t i = 2; i < imageViews.size(); ++i) {
      commandBuffer.imageMemoryBarrier(imageViews[i], memoryBarrier);
    }
  }
  {
    ImageMemoryBarrier memoryBarrier{};
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    memoryBarrier.srcAccess = 0;
    memoryBarrier.dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    memoryBarrier.srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    memoryBarrier.dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                             VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

    commandBuffer.imageMemoryBarrier(imageViews[1], memoryBarrier);
  }

  draw(commandBuffer, renderTarget);

  {
    ImageMemoryBarrier memoryBarrier{};
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    memoryBarrier.srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    memoryBarrier.srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    memoryBarrier.dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    commandBuffer.imageMemoryBarrier(imageViews[0], memoryBarrier);
  }
}

bool update() {
  CommandBuffer *commandBuffer{nullptr};
  if (!renderContext->begin(&commandBuffer)) { return false; }
  if (!commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
    return false;
  }
  render(*commandBuffer, renderContext->getActiveFrame()->getRenderTarget());
  if (!commandBuffer->end()) { return false; }
  if (!renderContext->submit(commandBuffer)) { return false; }
  return true;
}

int main() {
  printf("Hello, stranger.\n");

  if (!glfwInit() || !glfwVulkanSupported()) { return 1; }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  auto window = glfwCreateWindow(windowWidth, windowHeight, windowTitle,
                                 nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  if (!createInstance(&instance, &messenger)) { return 1; }

  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS) {
    return 1;
  }

  if (!createDevice(instance, &device, surface)) { return 1; }

  int width{0}, height{0};
  glfwGetFramebufferSize(window, &width, &height);

  auto swapchain =
      Swapchain::make(device, surface, {(uint32_t)width, (uint32_t)height}, 3);
  if (!swapchain) { return 1; }

  renderContext = RenderContext::make(std::move(swapchain));
  if (!renderContext) { return 1; }

  ShaderSource vertShader{};
  ShaderSource fragShader{};
  if (!createShaderSource(&vertShader, "base.vert") ||
      !createShaderSource(&fragShader, "base.frag")) {
    return 1;
  }

  auto sceneSubpass = std::make_unique<ForwardSubpass>(
      renderContext.get(), std::move(vertShader), std::move(fragShader));

  auto renderPipeline = std::make_unique<RenderPipeline>();
  renderPipeline->addSubpass(std::move(sceneSubpass));

  while (!glfwWindowShouldClose(window)) {
    if (!update()) { printf("update error\n"); }
    glfwPollEvents();
  }

  device.waitIdle();

  renderPipeline.reset();

  renderContext.reset();

  destroyDevice(&device);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  surface = VK_NULL_HANDLE;
  destroyInstance(&instance, &messenger);

  glfwDestroyWindow(window);
  glfwTerminate();

  printf("Bye.\n");
  return 0;
}
