#include "renderer/render_pass.h"
#include "renderer/device.h"
#include "renderer/render_target.h"

std::vector<VkAttachmentDescription>
getAttachmentDescriptions(const std::vector<Attachment> &attachments,
                          const std::vector<LoadStoreOp> &loadStoreOps) {
  std::vector<VkAttachmentDescription> attachmentDescriptions;

  for (size_t i = 0U; i < attachments.size(); ++i) {
    VkAttachmentDescription description{};
    description.format = attachments[i].format;
    description.samples = attachments[i].sampleCount;
    description.initialLayout = attachments[i].initialLayout;

    if (isDepthOnlyFormat(description.format)) {
      description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    } else if (isDepthStencilFormat(description.format)) {
      description.finalLayout =
          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    } else {
      description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    description.loadOp = loadStoreOps[i].loadOp;
    description.storeOp = loadStoreOps[i].storeOp;

    attachmentDescriptions.emplace_back(description);
  }

  return std::move(attachmentDescriptions);
}

VkAttachmentReference getAttachmentReference(const uint32_t attachment,
                                             const VkImageLayout layout) {
  VkAttachmentReference reference{.attachment = attachment, .layout = layout};
  return reference;
}

void setAttachmentLayouts(
    std::vector<VkSubpassDescription> &subpassDescriptions,
    std::vector<VkAttachmentDescription> &attachmentDescriptions) {
  // Make the initial layout same as in the first subpass using that attachment
  for (auto &subpass : subpassDescriptions) {
    for (size_t i = 0U; i < subpass.colorAttachmentCount; ++i) {
      const auto &reference = subpass.pColorAttachments[i];
      // Set it only if not defined yet
      if (attachmentDescriptions[reference.attachment].initialLayout ==
          VK_IMAGE_LAYOUT_UNDEFINED) {
        attachmentDescriptions[reference.attachment].initialLayout =
            reference.layout;
      }
    }

    for (size_t i = 0U; i < subpass.inputAttachmentCount; ++i) {
      const auto &reference = subpass.pInputAttachments[i];
      // Set it only if not defined yet
      if (attachmentDescriptions[reference.attachment].initialLayout ==
          VK_IMAGE_LAYOUT_UNDEFINED) {
        attachmentDescriptions[reference.attachment].initialLayout =
            reference.layout;
      }
    }

    if (subpass.pDepthStencilAttachment) {
      const auto &reference = *subpass.pDepthStencilAttachment;
      // Set it only if not defined yet
      if (attachmentDescriptions[reference.attachment].initialLayout ==
          VK_IMAGE_LAYOUT_UNDEFINED) {
        attachmentDescriptions[reference.attachment].initialLayout =
            reference.layout;
      }
    }
  }

  { // Make the final layout same as the last subpass layout
    auto &subpass = subpassDescriptions.back();

    for (size_t i = 0U; i < subpass.colorAttachmentCount; ++i) {
      const auto &reference = subpass.pColorAttachments[i];
      attachmentDescriptions[reference.attachment].finalLayout =
          reference.layout;
    }

    for (size_t i = 0U; i < subpass.inputAttachmentCount; ++i) {
      const auto &reference = subpass.pInputAttachments[i];

      attachmentDescriptions[reference.attachment].finalLayout =
          reference.layout;

      // Do not use depth attachment if used as input
      if (isDepthStencilFormat(
              attachmentDescriptions[reference.attachment].format)) {
        subpass.pDepthStencilAttachment = nullptr;
      }
    }

    if (subpass.pDepthStencilAttachment) {
      const auto &reference = *subpass.pDepthStencilAttachment;
      attachmentDescriptions[reference.attachment].finalLayout =
          reference.layout;
    }
  }
}

std::vector<VkSubpassDependency>
getSubpassDependencies(const size_t subpassCount) {
  std::vector<VkSubpassDependency> dependencies(subpassCount - 1);

  for (uint32_t i = 0; i < dependencies.size(); ++i) {
    // Transition input attachments from color attachment to shader read
    dependencies[i].srcSubpass = i;
    dependencies[i].dstSubpass = i + 1;
    dependencies[i].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  }

  return std::move(dependencies);
}

std::unique_ptr<RenderPass>
RenderPass::make(Device &device, const std::vector<Attachment> &attachments,
                 const std::vector<LoadStoreOp> &loadStoreOps,
                 const std::vector<SubpassInfo> &subpasses) {
  auto attachmentDescriptions =
      getAttachmentDescriptions(attachments, loadStoreOps);

  // Attachments for each subpass
  std::vector<std::vector<VkAttachmentReference>> inputAttachments{
      subpasses.size()};
  std::vector<std::vector<VkAttachmentReference>> colorAttachments{
      subpasses.size()};
  std::vector<std::vector<VkAttachmentReference>> depthStencilAttachments{
      subpasses.size()};

  for (size_t i = 0; i < subpasses.size(); ++i) {
    auto &subpass = subpasses[i];

    // fill color attachments references
    for (auto output : subpass.outputAttachments) {
      auto defaultLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      auto initialLayout =
          attachments[output].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
              ? defaultLayout
              : attachments[output].initialLayout;
      colorAttachments[i].emplace_back(
          getAttachmentReference(output, initialLayout));
    }

    // fill input attachments references
    for (auto input : subpass.inputAttachments) {
      auto defaultLayout =
          isDepthStencilFormat(attachmentDescriptions[input].format)
              ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
              : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      auto initialLayout =
          attachments[input].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
              ? defaultLayout
              : attachments[input].initialLayout;
      inputAttachments[i].emplace_back(
          getAttachmentReference(input, initialLayout));
    }

    if (subpass.enableDepthStencilAttachment) {
      auto it = find_if(attachments.begin(), attachments.end(),
                        [](const Attachment &attachment) {
                          return isDepthStencilFormat(attachment.format);
                        });
      if (it != attachments.end()) {
        auto index = std::distance(attachments.begin(), it);
        auto initialLayout =
            it->initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
                ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                : it->initialLayout;
        depthStencilAttachments[i].emplace_back(
            getAttachmentReference(index, initialLayout));
      }
    }
  }

  std::vector<VkSubpassDescription> subpassDescriptions;
  subpassDescriptions.reserve(subpasses.size());
  for (size_t i = 0; i < subpasses.size(); ++i) {
    auto &subpass = subpasses[i];

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpassDescription.pInputAttachments = inputAttachments[i].data();
    subpassDescription.inputAttachmentCount = inputAttachments[i].size();

    subpassDescription.pColorAttachments = colorAttachments[i].data();
    subpassDescription.colorAttachmentCount = colorAttachments[i].size();

    subpassDescription.pDepthStencilAttachment =
        depthStencilAttachments[i].data();

    subpassDescriptions.emplace_back(subpassDescription);
  }

  setAttachmentLayouts(subpassDescriptions, attachmentDescriptions);

  const auto &subpassDependencies = getSubpassDependencies(subpasses.size());

  VkRenderPassCreateInfo createInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  createInfo.attachmentCount = attachmentDescriptions.size();
  createInfo.pAttachments = attachmentDescriptions.data();
  createInfo.subpassCount = subpassDescriptions.size();
  createInfo.pSubpasses = subpassDescriptions.data();
  createInfo.dependencyCount = subpassDependencies.size();
  createInfo.pDependencies = subpassDependencies.data();

  VkRenderPass handle{VK_NULL_HANDLE};
  if (vkCreateRenderPass(device.handle, &createInfo, nullptr, &handle) !=
      VK_SUCCESS) {
    return nullptr;
  }

  auto renderPass = std::make_unique<RenderPass>(device);
  renderPass->handle = handle;
  return renderPass;
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(device.handle, handle, nullptr);
}
