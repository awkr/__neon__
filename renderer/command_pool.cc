#include "renderer/command_pool.h"
#include "renderer/device.h"

std::unique_ptr<CommandPool> CommandPool::make(Device      &device,
                                               uint32_t     queueFamilyIndex,
                                               RenderFrame *renderFrame,
                                               CommandBufferResetMode resetMode,
                                               size_t threadIndex) {
  VkCommandPoolCreateFlags flags;
  switch (resetMode) {
  case CommandBufferResetMode::ResetIndividually:
  case CommandBufferResetMode::AlwaysAllocate:
    flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    break;
  case CommandBufferResetMode::ResetPool:
  default:
    flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    break;
  }
  VkCommandPoolCreateInfo createInfo{
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  createInfo.queueFamilyIndex = queueFamilyIndex;
  createInfo.flags            = flags;

  VkCommandPool handle{VK_NULL_HANDLE};
  auto          result =
      vkCreateCommandPool(device.handle, &createInfo, nullptr, &handle);
  if (result != VK_SUCCESS) { return nullptr; }

  auto commandPool         = std::make_unique<CommandPool>(device);
  commandPool->renderFrame = renderFrame;
  commandPool->resetMode   = resetMode;
  commandPool->handle      = handle;
  commandPool->threadIndex = threadIndex;
  return std::move(commandPool);
}

CommandPool::~CommandPool() {
  primaryCommandBuffers.clear();
  secondaryCommandBuffers.clear();
  vkDestroyCommandPool(device.handle, handle, nullptr);
}

bool CommandPool::requestCommandBuffer(CommandBuffer      **commandBuffer,
                                       VkCommandBufferLevel level) {
  if (level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
    if (activePrimaryCommandBufferCount < primaryCommandBuffers.size()) {
      *commandBuffer =
          primaryCommandBuffers[activePrimaryCommandBufferCount++].get();
      return true;
    }

    auto command = CommandBuffer::make(*this, level);
    if (!command) { return false; }

    primaryCommandBuffers.emplace_back(std::move(command));
    ++activePrimaryCommandBufferCount;
    *commandBuffer = primaryCommandBuffers.back().get();
    return true;
  }

  if (activeSecondaryCommandBufferCount < secondaryCommandBuffers.size()) {
    *commandBuffer =
        secondaryCommandBuffers[activeSecondaryCommandBufferCount++].get();
    return true;
  }

  auto command = CommandBuffer::make(*this, level);
  if (!command) { return false; }

  secondaryCommandBuffers.emplace_back(std::move(command));
  ++activeSecondaryCommandBufferCount;
  *commandBuffer = secondaryCommandBuffers.back().get();
  return true;
}

void CommandPool::resetPool() {
  switch (resetMode) {
  case CommandBufferResetMode::ResetPool:
    resetCommandBuffers();
    break;
  case CommandBufferResetMode::ResetIndividually:
    // TODO
    break;
  case CommandBufferResetMode::AlwaysAllocate:
    // TODO
    break;
  }
}

void CommandPool::resetCommandBuffers() {
  for (auto &commandBuffer : primaryCommandBuffers) {
    commandBuffer->reset(resetMode);
  }
}
