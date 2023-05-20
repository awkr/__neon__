#include "renderer/command_buffer.h"
#include "renderer/command_pool.h"
#include "renderer/device.h"

std::unique_ptr<CommandBuffer> CommandBuffer::make(CommandPool *commandPool,
                                                   VkCommandBufferLevel level) {
  VkCommandBufferAllocateInfo allocateInfo{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocateInfo.commandPool = commandPool->handle;
  allocateInfo.commandBufferCount = 1;
  allocateInfo.level = level;

  VkCommandBuffer handle{VK_NULL_HANDLE};
  if (vkAllocateCommandBuffers(commandPool->device.handle, &allocateInfo,
                               &handle) != VK_SUCCESS) {
    return nullptr;
  }

  auto commandBuffer = std::make_unique<CommandBuffer>();
  commandBuffer->handle = handle;
  return std::move(commandBuffer);
}
