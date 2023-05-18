#include "renderer/device.h"
#include "renderer/instance.h"
#include "renderer/render_context.h"
#include "renderer/shader_module.h"
#include <GLFW/glfw3.h>
#include <cstdio>

const char *windowTitle = "neon";
uint32_t windowWidth{800};
uint32_t windowHeight{500};

VkInstance instance{VK_NULL_HANDLE};
VkDebugUtilsMessengerEXT messenger{VK_NULL_HANDLE};
VkSurfaceKHR surface{VK_NULL_HANDLE};
Device device{};
Swapchain swapchain{};

RenderContext renderContext{};

int main() {
  printf("hello, stranger.\n");

  if (!glfwInit() || !glfwVulkanSupported()) {
    return 1;
  }

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

  if (!createInstance(&instance, &messenger)) {
    return 1;
  }

  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS) {
    return 1;
  }

  if (!createDevice(instance, &device)) {
    return 1;
  }

  int renderWidth{0}, renderHeight{0};
  glfwGetFramebufferSize(window, &renderWidth, &renderHeight);

  if (!createSwapchain(&swapchain, &device, surface,
                       {(uint32_t)renderWidth, (uint32_t)renderHeight}, 3)) {
    return 1;
  }

  if (!createRenderContext(&renderContext, &swapchain)) {
    return 1;
  }

  ShaderSource vertShader{};
  ShaderSource fragShader{};
  if (!createShaderSource(&vertShader, "base.vert") ||
      !createShaderSource(&fragShader, "base.frag")) {
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  destroyRenderContext(&renderContext);

  destroySwapchain(device.handle, &swapchain);
  destroyDevice(&device);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  surface = VK_NULL_HANDLE;
  destroyInstance(&instance, &messenger);

  glfwDestroyWindow(window);
  glfwTerminate();

  printf("bye.\n");
  return 0;
}
