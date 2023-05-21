#include "renderer/command_buffer.h"
#include "renderer/command_pool.h"
#include "renderer/device.h"
#include "renderer/image_view.h"

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
  commandBuffer->level = level;
  commandBuffer->handle = handle;
  return std::move(commandBuffer);
}

void CommandBuffer::reset(CommandBufferResetMode resetMode) {
  // TODO
}

bool CommandBuffer::begin(VkCommandBufferUsageFlags usage) {
  if (state == CommandBufferState::Recording) { return false; }
  state = CommandBufferState::Recording;

  // TODO reset each domain state

  VkCommandBufferBeginInfo beginInfo{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  beginInfo.flags = usage;
  // VkCommandBufferInheritanceInfo inheritanceInfo{
  //     VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO};
  // if (level == VK_COMMAND_BUFFER_LEVEL_SECONDARY) {}
  return vkBeginCommandBuffer(handle, &beginInfo) == VK_SUCCESS;
}

bool CommandBuffer::end() {
  if (state != CommandBufferState::Recording) { return false; }
  if (vkEndCommandBuffer(handle) != VK_SUCCESS) { return false; }
  state = CommandBufferState::Executable;
  return true;
}

void CommandBuffer::imageMemoryBarrier(
    const ImageView &imageView, const ImageMemoryBarrier &memoryBarrier) const {
  VkImageMemoryBarrier imageMemoryBarrier{
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
  imageMemoryBarrier.oldLayout = memoryBarrier.oldLayout;
  imageMemoryBarrier.newLayout = memoryBarrier.newLayout;
  imageMemoryBarrier.image = imageView.image->handle;
  imageMemoryBarrier.subresourceRange = imageView.subresourceRange;
  imageMemoryBarrier.srcAccessMask = memoryBarrier.srcAccess;
  imageMemoryBarrier.dstAccessMask = memoryBarrier.dstAccess;
  imageMemoryBarrier.srcQueueFamilyIndex = memoryBarrier.oldQueueFamily;
  imageMemoryBarrier.dstQueueFamilyIndex = memoryBarrier.newQueueFamily;

  VkPipelineStageFlags srcStage = memoryBarrier.srcStage;
  VkPipelineStageFlags dstStage = memoryBarrier.dstStage;

  vkCmdPipelineBarrier(handle, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1,
                       &imageMemoryBarrier);
}

void CommandBuffer::setViewport(const VkViewport &viewport) const {
  std::vector<VkViewport> viewports{viewport};
  vkCmdSetViewport(handle, 0, viewports.size(), viewports.data());
}

void CommandBuffer::setScissor(const VkRect2D &scissor) const {
  std::vector<VkRect2D> scissors{scissor};
  vkCmdSetScissor(handle, 0, scissors.size(), scissors.data());
}
