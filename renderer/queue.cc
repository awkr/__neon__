#include "renderer/queue.h"

bool Queue::submit(const std::vector<VkSubmitInfo> &submitInfos,
                   VkFence                          fence) const {
  return vkQueueSubmit(handle, submitInfos.size(), submitInfos.data(), fence) ==
         VK_SUCCESS;
}

bool Queue::present(const VkPresentInfoKHR &presentInfo) const {
  return vkQueuePresentKHR(handle, &presentInfo) == VK_SUCCESS;
}
