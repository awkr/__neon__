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

bool update() {
  CommandBuffer *commandBuffer{nullptr};
  if (!renderContext->begin(&commandBuffer)) { return false; }
  if (!commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
    return false;
  }
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
