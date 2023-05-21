#pragma once

#include <vulkan/vulkan.h>

// define memory access for an image view during command recording
struct ImageMemoryBarrier {
  VkPipelineStageFlags srcStage{VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
  VkPipelineStageFlags dstStage{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
  VkAccessFlags srcAccess{0};
  VkAccessFlags dstAccess{0};
  VkImageLayout oldLayout{VK_IMAGE_LAYOUT_UNDEFINED};
  VkImageLayout newLayout{VK_IMAGE_LAYOUT_UNDEFINED};
  uint32_t oldQueueFamily{VK_QUEUE_FAMILY_IGNORED};
  uint32_t newQueueFamily{VK_QUEUE_FAMILY_IGNORED};
};

// load and store operation for a render pass attachment
struct LoadStoreOp {
  VkAttachmentLoadOp loadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
  VkAttachmentStoreOp storeOp{VK_ATTACHMENT_STORE_OP_STORE};
};
