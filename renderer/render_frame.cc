#include "renderer/render_frame.h"
#include "renderer/device.h"
#include "renderer/resource_cache.h"

RenderFrame::RenderFrame(Device                         &device,
                         std::unique_ptr<RenderTarget> &&renderTarget,
                         size_t                          threadCount)
    : device{device}, semaphorePool{device}, fencePool{device},
      renderTarget{std::move(renderTarget)}, threadCount{threadCount} {
  for (size_t i = 0; i < threadCount; ++i) {
    descriptorPools.push_back(
        std::make_unique<std::unordered_map<
            std::size_t, std::unique_ptr<DescriptorPool>>>());
    descriptorSets.push_back(
        std::make_unique<
            std::unordered_map<std::size_t, std::unique_ptr<DescriptorSet>>>());
  }
}

RenderFrame::~RenderFrame() { renderTarget.reset(); }

bool RenderFrame::requestOutSemaphore(VkSemaphore &semaphore) {
  return semaphorePool.requestOutSemaphore(semaphore);
}

bool RenderFrame::requestSemaphore(VkSemaphore &semaphore) {
  return semaphorePool.requestSemaphore(semaphore);
}

void RenderFrame::releaseSemaphore(VkSemaphore semaphore) {
  semaphorePool.releaseSemaphore(semaphore);
}

bool RenderFrame::requestCommandBuffer(CommandBuffer        **commandBuffer,
                                       const Queue           *queue,
                                       CommandBufferResetMode resetMode,
                                       VkCommandBufferLevel   level,
                                       size_t                 threadIndex) {
  std::vector<std::unique_ptr<CommandPool>> *commandPool{nullptr};
  bool ok = getCommandPool(queue, resetMode, &commandPool);
  if (!ok) { return false; }
  auto it =
      std::find_if(commandPool->begin(), commandPool->end(),
                   [&threadIndex](std::unique_ptr<CommandPool> &commandPool) {
                     return commandPool->threadIndex == threadIndex;
                   });
  return (*it)->requestCommandBuffer(commandBuffer, level);
}

VkDescriptorSet RenderFrame::requestDescriptorSet(
    const DescriptorSetLayout                &descriptorSetLayout,
    const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
    const BindingMap<VkDescriptorImageInfo> &imageInfos, bool updateAfterBind,
    size_t threadIndex) const {
  auto descriptorPool = requestResource(*descriptorPools[threadIndex], device,
                                        descriptorSetLayout);
  if (descriptorManagementStrategy ==
      DescriptorManagementStrategy::StoreInCache) {
    std::vector<uint32_t> bindingsToUpdate;
    if (updateAfterBind) {
      bindingsToUpdate =
          collectBindingsToUpdate(descriptorSetLayout, bufferInfos, imageInfos);
    }

    auto descriptorSet = requestResource(*descriptorSets[threadIndex], device,
                                         descriptorSetLayout, *descriptorPool,
                                         bufferInfos, imageInfos);
    descriptorSet->update(bindingsToUpdate);
    return descriptorSet->handle;
  } else {
    DescriptorSet descriptorSet{device, descriptorSetLayout, *descriptorPool,
                                bufferInfos, imageInfos};
    descriptorSet.applyWrites();
    return descriptorSet.handle;
  }
}

bool RenderFrame::requestFence(VkFence &fence) {
  return fencePool.requestFence(fence);
}

void RenderFrame::reset() {
  fencePool.wait();
  fencePool.reset();
  for (auto &queueCommandPools : commandPools) {
    for (auto &commandPool : queueCommandPools.second) {
      commandPool->resetPool();
    }
  }
  semaphorePool.reset();
}

bool RenderFrame::getCommandPool(
    const Queue *queue, CommandBufferResetMode resetMode,
    std::vector<std::unique_ptr<CommandPool>> **commandPool) {
  if (auto it = commandPools.find(queue->familyIndex);
      it != commandPools.end()) {
    if (it->second.front()->resetMode != resetMode) {
      device.waitIdle();
      commandPools.erase(it); // Delete pools
    } else {
      *commandPool = &(it->second);
      return true;
    }
  }
  std::vector<std::unique_ptr<CommandPool>> queueCommandPools;
  for (size_t i = 0; i < threadCount; ++i) {
    auto pool = CommandPool::make(device, queue->familyIndex, this, resetMode);
    if (!pool) { return false; }
    queueCommandPools.emplace_back(std::move(pool));
  }

  auto it =
      commandPools.emplace(queue->familyIndex, std::move(queueCommandPools));
  if (!it.second) { return false; }
  *commandPool = &(it.first->second);
  return true;
}

void RenderFrame::updateRenderTarget(
    std::unique_ptr<RenderTarget> &&renderTarget) {
  this->renderTarget = std::move(renderTarget);
}

std::vector<uint32_t> RenderFrame::collectBindingsToUpdate(
    const DescriptorSetLayout                &descriptorSetLayout,
    const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
    const BindingMap<VkDescriptorImageInfo>  &imageInfos) {
  std::vector<uint32_t> bindingsToUpdate;
  bindingsToUpdate.reserve(bufferInfos.size() + imageInfos.size());
  auto aggregateBindingToUpdate = [&bindingsToUpdate,
                                   &descriptorSetLayout](const auto &infos) {
    for (const auto &pair : infos) {
      auto bindingIndex = pair.first;
      if (!(descriptorSetLayout.getLayoutBindingFlag(bindingIndex) &
            VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) &&
          std::find(bindingsToUpdate.begin(), bindingsToUpdate.end(),
                    bindingIndex) == bindingsToUpdate.end()) {
        bindingsToUpdate.push_back(bindingIndex);
      }
    }
  };
  aggregateBindingToUpdate(bufferInfos);
  aggregateBindingToUpdate(imageInfos);

  return bindingsToUpdate;
}
