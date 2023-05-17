#include <GLFW/glfw3.h>
#include <cstdio>

const char *windowTitle = "neon";
uint32_t windowWidth{800};
uint32_t windowHeight{500};

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

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
