cmake_minimum_required(VERSION 3.21)

project(neon)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(third-party/glfw)

add_executable(neon main.cc)

target_link_libraries(neon PRIVATE glfw)

#target_compile_options(neon PRIVATE
#    GLFW_INCLUDE_VULKAN)
