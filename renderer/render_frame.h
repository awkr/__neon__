#pragma once

#include "renderer/command_pool.h"
#include "renderer/fence_pool.h"
#include "renderer/render_target.h"
#include "renderer/semaphore_pool.h"

struct CommandBuffer;
struct Queue;
struct DescriptorPool;
struct DescriptorSet;

enum class DescriptorManagementStrategy {
  StoreInCache,
  CreateDirectly,
};

struct RenderFrame {
  RenderFrame(Device &device, std::unique_ptr<RenderTarget> &&renderTarget,
              size_t threadCount = 1);

  ~RenderFrame();

  bool requestOutSemaphore(VkSemaphore &semaphore);
  bool requestSemaphore(VkSemaphore &semaphore);
  void releaseSemaphore(VkSemaphore semaphore);

  bool requestCommandBuffer(
      CommandBuffer **commandBuffer, const Queue *queue,
      CommandBufferResetMode resetMode   = CommandBufferResetMode::ResetPool,
      VkCommandBufferLevel   level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      size_t                 threadIndex = 0);

  VkDescriptorSet
  requestDescriptorSet(const DescriptorSetLayout &descriptorSetLayout,
                       const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
                       const BindingMap<VkDescriptorImageInfo>  &imageInfos,
                       bool updateAfterBind, size_t threadIndex = 0) const;

  bool requestFence(VkFence &fence);
  void reset();

  RenderTarget *getRenderTarget() { return renderTarget.get(); }
  void updateRenderTarget(std::unique_ptr<RenderTarget> &&renderTarget);

private:
  bool getCommandPool(const Queue *queue, CommandBufferResetMode resetMode,
                      std::vector<std::unique_ptr<CommandPool>> **commandPool);

  static std::vector<uint32_t>
  collectBindingsToUpdate(const DescriptorSetLayout                &infos,
                          const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
                          const BindingMap<VkDescriptorImageInfo>  &imageInfos);

  Device                       &device;
  std::unique_ptr<RenderTarget> renderTarget;
  size_t                        threadCount{1};
  SemaphorePool                 semaphorePool;
  FencePool                     fencePool;
  std::unordered_map<uint32_t, std::vector<std::unique_ptr<CommandPool>>>
      commandPools; // Key is queue family index
  std::vector<std::unique_ptr<
      std::unordered_map<std::size_t, std::unique_ptr<DescriptorPool>>>>
      descriptorPools;
  std::vector<std::unique_ptr<
      std::unordered_map<std::size_t, std::unique_ptr<DescriptorSet>>>>
                               descriptorSets;
  DescriptorManagementStrategy descriptorManagementStrategy{
      DescriptorManagementStrategy::StoreInCache};
};
