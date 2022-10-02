#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "device.h"
#include "spinlock.h"

namespace vkl {

class VulkanFramebuffer : public FrameBuffer {
public:
    static VkResult create(VulkanDevice *device, const FramebufferCreateInfo *pCreateInfo, VulkanFramebuffer **ppFramebuffer, uint32_t attachmentCount, VulkanImageView **ppAttachments);

    ~VulkanFramebuffer() = default;

    VkFramebuffer getHandle(VulkanRenderPass* pRenderPass);

    uint32_t GetAttachmentCount() const;

    VkExtent2D GetExtents();

    VulkanImageView *GetAttachment(uint32_t attachmentIndex);

private:
    VulkanDevice                                         *m_device = nullptr;
    const void                                           *m_pNext;
    std::vector<VulkanImageView *>                        m_attachments;
    std::unordered_map<VulkanRenderPass *, VkFramebuffer> m_cache;
    SpinLock                                              m_spinLock;
};
} // namespace vkl

#endif // FRAMEBUFFER_H_
