#pragma once

#include "renderer/resource.h"
#include "renderer/types.h"
#include <vector>

struct Device;
struct Attachment;

struct SubpassInfo {
  std::vector<uint32_t> inputAttachments;
  std::vector<uint32_t> outputAttachments;
  bool                  enableDepthStencilAttachment{true};
};

struct RenderPass : public Resource {
  static std::unique_ptr<RenderPass>
  make(Device &device, const std::vector<Attachment> &attachments,
       const std::vector<LoadStoreOp> &loadStoreOps,
       const std::vector<SubpassInfo> &subpasses);

  RenderPass(Device &device) : device{device} {}
  ~RenderPass() override;

  uint32_t getColorOutputCount(uint32_t subpassIndex) const;

  VkRenderPass getHandle() const { return handle; }

  Device               &device;
  VkRenderPass          handle{VK_NULL_HANDLE};
  std::vector<uint32_t> colorOutputCount;
};
