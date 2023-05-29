#include "renderer/command_buffer.h"
#include "renderer/buffer.h"
#include "renderer/command_pool.h"
#include "renderer/device.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/subpass.h"
#include <unordered_set>

bool isDynamicBufferDescriptorType(VkDescriptorType descriptorType) {
  return descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC ||
         descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
}

bool isBufferDescriptorType(VkDescriptorType descriptorType) {
  return descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
         descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
         isDynamicBufferDescriptorType(descriptorType);
}

std::unique_ptr<CommandBuffer> CommandBuffer::make(CommandPool &commandPool,
                                                   VkCommandBufferLevel level) {
  VkCommandBufferAllocateInfo allocateInfo{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocateInfo.commandPool        = commandPool.handle;
  allocateInfo.commandBufferCount = 1;
  allocateInfo.level              = level;

  VkCommandBuffer handle{VK_NULL_HANDLE};
  if (vkAllocateCommandBuffers(commandPool.device.getHandle(), &allocateInfo,
                               &handle) != VK_SUCCESS) {
    return nullptr;
  }

  auto commandBuffer    = std::make_unique<CommandBuffer>(commandPool.device);
  commandBuffer->level  = level;
  commandBuffer->handle = handle;
  return std::move(commandBuffer);
}

void CommandBuffer::reset(CommandBufferResetMode resetMode) {
  // TODO
}

bool CommandBuffer::begin(VkCommandBufferUsageFlags usage) const {
  VkCommandBufferBeginInfo beginInfo{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  beginInfo.flags = usage;

  // TODO Secondary command buffer

  return vkBeginCommandBuffer(handle, &beginInfo) == VK_SUCCESS;
}

bool CommandBuffer::end() const {
  return vkEndCommandBuffer(handle) == VK_SUCCESS;
}

void CommandBuffer::imageMemoryBarrier(
    const ImageView &imageView, const ImageMemoryBarrier &memoryBarrier) const {
  VkImageMemoryBarrier imageMemoryBarrier{
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
  imageMemoryBarrier.oldLayout           = memoryBarrier.oldLayout;
  imageMemoryBarrier.newLayout           = memoryBarrier.newLayout;
  imageMemoryBarrier.image               = imageView.image->handle;
  imageMemoryBarrier.subresourceRange    = imageView.subresourceRange;
  imageMemoryBarrier.srcAccessMask       = memoryBarrier.srcAccess;
  imageMemoryBarrier.dstAccessMask       = memoryBarrier.dstAccess;
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

bool CommandBuffer::beginRenderPass(
    const RenderTarget                          &renderTarget,
    const std::vector<LoadStoreOp>              &loadStoreOps,
    const std::vector<VkClearValue>             &clearValues,
    const std::vector<std::unique_ptr<Subpass>> &subpasses) {
  // Get render pass
  std::vector<SubpassInfo> subpassInfos(subpasses.size());
  auto                     it = subpassInfos.begin();
  for (const auto &subpass : subpasses) {
    it->inputAttachments             = subpass->getInputAttachments();
    it->outputAttachments            = subpass->getOutputAttachments();
    it->enableDepthStencilAttachment = subpass->enableDepthStencilAttachment;

    ++it;
  }
  auto renderPass = device.getResourceCache().requestRenderPass(
      renderTarget.attachments, loadStoreOps, subpassInfos);
  if (!renderPass) { return false; }

  // Get framebuffer
  auto framebuffer =
      device.getResourceCache().requestFramebuffer(renderTarget, *renderPass);
  if (!framebuffer) { return false; }

  beginRenderPass(*renderPass, renderTarget, *framebuffer, clearValues);

  return true;
}

void CommandBuffer::endRenderPass() const { vkCmdEndRenderPass(handle); }

void CommandBuffer::nextSubpass() {
  pipelineState.setSubpassIndex(pipelineState.subpassIndex + 1);

  // Update blend state attachments
  auto blendState       = pipelineState.getColorBlendState();
  auto colorOutputCount = renderPassBinding.renderPass->getColorOutputCount(
      pipelineState.subpassIndex);
  blendState.attachments.resize(colorOutputCount);
  pipelineState.setColorBlendState(blendState);

  vkCmdNextSubpass(handle, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::setRasterizationState(const RasterizationState &state) {
  pipelineState.setRasterizationState(state);
}

void CommandBuffer::setMultisampleState(const MultisampleState &state) {
  pipelineState.setMultisampleState(state);
}

void CommandBuffer::setVertexInputState(const VertexInputState &state) {
  pipelineState.setVertexInputState(state);
}

void CommandBuffer::beginRenderPass(
    const RenderPass &renderPass, const RenderTarget &renderTarget,
    const Framebuffer               &framebuffer,
    const std::vector<VkClearValue> &clearValues) {
  renderPassBinding.renderPass  = &renderPass;
  renderPassBinding.framebuffer = &framebuffer;

  VkRenderPassBeginInfo beginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  beginInfo.renderPass        = renderPass.getHandle();
  beginInfo.framebuffer       = framebuffer.getHandle();
  beginInfo.renderArea.extent = renderTarget.getExtent();
  beginInfo.clearValueCount   = clearValues.size();
  beginInfo.pClearValues      = clearValues.data();

  vkCmdBeginRenderPass(handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

  // Update blend state attachments for first subpass
  auto blendState = pipelineState.getColorBlendState();
  auto colorOutputCount =
      renderPass.getColorOutputCount(pipelineState.subpassIndex); // should be 0
  blendState.attachments.resize(colorOutputCount);
  pipelineState.setColorBlendState(blendState);
}

void CommandBuffer::bindPipelineLayout(const PipelineLayout &pipelineLayout) {
  pipelineState.setPipelineLayout(pipelineLayout);
}

void CommandBuffer::bindVertexBuffers(
    uint32_t                                                 firstBinding,
    const std::vector<std::reference_wrapper<const Buffer>> &buffers,
    const std::vector<VkDeviceSize>                         &offsets) const {
  std::vector<VkBuffer> handles(buffers.size(), VK_NULL_HANDLE);
  std::transform(buffers.begin(), buffers.end(), handles.begin(),
                 [](const Buffer &buffer) { return buffer.handle; });
  vkCmdBindVertexBuffers(handle, firstBinding, handles.size(), handles.data(),
                         offsets.data());
}

void CommandBuffer::bindIndexBuffer() {}

void CommandBuffer::flush(VkPipelineBindPoint pipelineBindPoint) {
  flushPipelineState(pipelineBindPoint);
  flushPushConstants();
  flushDescriptorState(pipelineBindPoint);
}

void CommandBuffer::drawIndexed() {}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount,
                         uint32_t firstVertex, uint32_t firstInstance) {
  flush(VK_PIPELINE_BIND_POINT_GRAPHICS);
  vkCmdDraw(handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::flushPipelineState(VkPipelineBindPoint pipelineBindPoint) {
  // Create a new pipeline only if graphics state changed
  if (!pipelineState.isDirty()) { return; }

  pipelineState.clearDirty();

  // Create and bind pipeline
  if (pipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) {
    pipelineState.setRenderPass(*renderPassBinding.renderPass);
    auto pipeline =
        device.getResourceCache().requestGraphicsPipeline(pipelineState);

    vkCmdBindPipeline(handle, pipelineBindPoint, pipeline->getHandle());
  } else if (pipelineBindPoint == VK_PIPELINE_BIND_POINT_COMPUTE) {
    // TODO
  }
}

void CommandBuffer::flushPushConstants() {
  if (storedPushConstants.empty()) { return; }
  auto pipelineLayout = pipelineState.getPipelineLayout();
  auto shaderStages =
      pipelineLayout->getPushConstantRangeStage(storedPushConstants.size());
  vkCmdPushConstants(handle, pipelineLayout->handle, shaderStages, 0,
                     storedPushConstants.size(), storedPushConstants.data());
  storedPushConstants.clear();
}

void CommandBuffer::flushDescriptorState(
    VkPipelineBindPoint pipelineBindPoint) {
  // Check if a descriptor set needs to be created
  if (!resourceBindingState.dirty) { return; }

  resourceBindingState.clearDirty();

  const auto pipelineLayout = pipelineState.getPipelineLayout();

  for (auto &[setIndex, resourceSet] : resourceBindingState.resourceSets) {
    if (!resourceSet.dirty) { continue; }

    resourceSet.clearDirty();

    const auto &descriptorSetLayout =
        pipelineLayout->getDescriptorSetLayout(setIndex);

    BindingMap<VkDescriptorBufferInfo> bufferInfos;
    BindingMap<VkDescriptorImageInfo>  imageInfos;

    std::vector<uint32_t> dynamicOffsets;

    for (auto &iter : resourceSet.getResourceBindings()) {
      auto  bindingIndex     = iter.first;
      auto &bindingResources = iter.second;

      // Check if binding exists in the pipeline layout
      if (auto bindingInfo =
              descriptorSetLayout->getLayoutBinding(bindingIndex)) {
        for (auto &[arrayElement, resourceInfo] : bindingResources) {
          // Pointer references
          auto &buffer    = resourceInfo.buffer;
          auto &sampler   = resourceInfo.sampler;
          auto &imageView = resourceInfo.imageView;

          if (buffer && isBufferDescriptorType(bindingInfo->descriptorType)) {
            VkDescriptorBufferInfo bufferInfo{};

            bufferInfo.buffer = resourceInfo.buffer->handle;
            bufferInfo.offset = resourceInfo.offset;
            bufferInfo.range  = resourceInfo.range;

            if (isDynamicBufferDescriptorType(bindingInfo->descriptorType)) {
              dynamicOffsets.push_back(bufferInfo.offset);

              bufferInfo.offset = 0;
            }

            bufferInfos[bindingIndex][arrayElement] = std::move(bufferInfo);
          } else if (imageView || sampler) {
            // Can be null for input attachments
            VkDescriptorImageInfo imageInfo{};
            imageInfo.sampler   = sampler ? sampler->handle : VK_NULL_HANDLE;
            imageInfo.imageView = imageView->handle;

            if (imageView) {
              // Add image layout info based on descriptor type
              switch (bindingInfo->descriptorType) {
              case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                imageInfo.imageLayout =
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                break;
              case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                if (isDepthStencilFormat(imageView->format)) {
                  imageInfo.imageLayout =
                      VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                } else {
                  imageInfo.imageLayout =
                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }
                break;
              case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                break;
              default:
                continue;
              }
            }

            imageInfos[bindingIndex][arrayElement] = imageInfo;
          }
        }
      }
    }

    auto descriptorSetHandle =
        commandPool->getRenderFrame()->requestDescriptorSet(
            *descriptorSetLayout, bufferInfos, imageInfos, updateAfterBind,
            commandPool->threadIndex);

    vkCmdBindDescriptorSets(
        handle, pipelineBindPoint, pipelineLayout->getHandle(), setIndex, 1,
        &descriptorSetHandle, dynamicOffsets.size(), dynamicOffsets.data());
  }
}
