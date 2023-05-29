#include "renderer/descriptor_set.h"
#include "renderer/device.h"
#include <iostream>

std::unique_ptr<DescriptorSet>
DescriptorSet::make(Device                    &device,
                    const DescriptorSetLayout &descriptorSetLayout,
                    const DescriptorPool      &descriptorPool,
                    const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
                    const BindingMap<VkDescriptorImageInfo>  &imageInfos) {
  return std::unique_ptr<DescriptorSet>();
}

DescriptorSet::DescriptorSet(
    Device &device, const DescriptorSetLayout &descriptorSetLayout,
    DescriptorPool                           &descriptorPool,
    const BindingMap<VkDescriptorBufferInfo> &bufferInfos,
    const BindingMap<VkDescriptorImageInfo>  &imageInfos)
    : device{device} {
  handle = descriptorPool.allocate();

  prepare();
}

bool DescriptorSet::prepare() {
  // don't prepare twice during the life cycle of a Descriptor Set
  if (!writeDescriptorSets.empty()) {
    std::cout << "Trying to prepare a descriptor set that has already been "
                 "prepared, skipping."
              << std::endl;
    return false;
  }

  for (auto &[bindingIndex, bufferBindings] : bufferInfos) {
    auto bindingInfo = descriptorSetLayout->getLayoutBinding(bindingIndex);
    if (!bindingInfo) { continue; }
    for (auto &it : bufferBindings) {
      auto &bufferInfo = it.second;

      size_t uniformBufferRangeLimit =
          device.physicalDevice.properties.limits.maxUniformBufferRange;
      size_t storageBufferRangeLimit =
          device.physicalDevice.properties.limits.maxStorageBufferRange;

      auto bufferRangeLimit = static_cast<size_t>(bufferInfo.range);

      if ((bindingInfo->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
           bindingInfo->descriptorType ==
               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) &&
          bufferRangeLimit > uniformBufferRangeLimit) {
        printf("Set %d binding %d cannot be updated: buffer size %llu exceeds "
               "the uniform buffer range limit %zu",
               descriptorSetLayout->getIndex(), bindingIndex, bufferInfo.range,
               uniformBufferRangeLimit);
        bufferRangeLimit = uniformBufferRangeLimit;
      } else if ((bindingInfo->descriptorType ==
                      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                  bindingInfo->descriptorType ==
                      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) &&
                 bufferRangeLimit > storageBufferRangeLimit) {
        printf("Set %d binding %d cannot be updated: buffer size %llu exceeds "
               "the storage buffer range limit %zu",
               descriptorSetLayout->getIndex(), bindingIndex, bufferInfo.range,
               storageBufferRangeLimit);
        bufferRangeLimit = storageBufferRangeLimit;
      }

      bufferInfo.range = bufferRangeLimit; // Clip range to the limit

      VkWriteDescriptorSet writeDescriptorSet{
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

      writeDescriptorSet.dstBinding      = bindingIndex;
      writeDescriptorSet.descriptorType  = bindingInfo->descriptorType;
      writeDescriptorSet.pBufferInfo     = &bufferInfo;
      writeDescriptorSet.dstSet          = handle;
      writeDescriptorSet.dstArrayElement = it.first;
      writeDescriptorSet.descriptorCount = 1;

      writeDescriptorSets.push_back(writeDescriptorSet);
    }
  }

  for (auto &[bindingIndex, bindingResources] : imageInfos) {
    auto bindingInfo = descriptorSetLayout->getLayoutBinding(bindingIndex);
    if (!bindingInfo) { continue; }

    for (auto &it : bindingResources) {
      auto &imageInfo = it.second;

      VkWriteDescriptorSet writeDescriptorSet{
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

      writeDescriptorSet.dstBinding      = bindingIndex;
      writeDescriptorSet.descriptorType  = bindingInfo->descriptorType;
      writeDescriptorSet.pImageInfo      = &imageInfo;
      writeDescriptorSet.dstSet          = handle;
      writeDescriptorSet.dstArrayElement = it.first;
      writeDescriptorSet.descriptorCount = 1;

      writeDescriptorSets.push_back(writeDescriptorSet);
    }
  }

  return true;
}

void DescriptorSet::applyWrites() const {
  vkUpdateDescriptorSets(device.handle, writeDescriptorSets.size(),
                         writeDescriptorSets.data(), 0, nullptr);
}

void DescriptorSet::update(const std::vector<uint32_t> &bindingsToUpdate) {
  std::vector<VkWriteDescriptorSet> writeOperations;
  std::vector<size_t>               writeOperationHashes;

  if (bindingsToUpdate.empty()) {
    for (const auto &writeOperation : writeDescriptorSets) {
      size_t writeOperationHash = 0;
      hashParam(writeOperationHash, writeOperation);

      auto it = updatedBindings.find(writeOperation.dstBinding);
      if (it == updatedBindings.end() || it->second != writeOperationHash) {
        writeOperations.push_back(writeOperation);
        writeOperationHashes.push_back(writeOperationHash);
      }
    }
  } else {
    for (const auto &writeOperation : writeDescriptorSets) {
      if (std::find(bindingsToUpdate.begin(), bindingsToUpdate.end(),
                    writeOperation.dstBinding) != bindingsToUpdate.end()) {
        size_t writeOperationHash = 0;
        hashParam(writeOperationHash, writeOperation);

        auto it = updatedBindings.find(writeOperation.dstBinding);
        if (it == updatedBindings.end() || it->second != writeOperationHash) {
          writeOperations.push_back(writeOperation);
          writeOperationHashes.push_back(writeOperationHash);
        }
      }
    }
  }

  if (!writeOperations.empty()) {
    vkUpdateDescriptorSets(device.handle, writeOperations.size(),
                           writeOperations.data(), 0, nullptr);
  }

  for (size_t i = 0; i < writeOperations.size(); ++i) {
    updatedBindings[writeOperations[i].dstBinding] = writeOperationHashes[i];
  }
}
