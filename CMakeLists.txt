cmake_minimum_required(VERSION 3.21)

project(neon)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(third-party/glfw)
add_subdirectory(third-party/glm)

set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "")
set(ENABLE_CTEST OFF CACHE BOOL "")

add_subdirectory(third-party/glslang)

find_package(Vulkan REQUIRED)

add_executable(neon
    main.cc
    renderer/instance.cc
    renderer/device.cc
    renderer/swapchain.cc
    renderer/image.cc
    renderer/render_target.cc
    renderer/ostream.cc
    renderer/render_context.cc
    renderer/render_frame.cc
    renderer/image_view.cc
    renderer/shader_module.cc
    renderer/forward_subpass.cc
    renderer/geometry_subpass.cc
    renderer/subpass.cc
    renderer/render_pipeline.cc
    renderer/resource_cache.cc
    renderer/semaphore_pool.cc
    renderer/queue.cc
    renderer/command_pool.cc
    renderer/command_buffer.cc
    renderer/fence_pool.cc)

target_link_libraries(neon PRIVATE glfw Vulkan::Vulkan glm glslang glslang-default-resource-limits SPIRV)

target_include_directories(neon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
