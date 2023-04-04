#include "commandPool.h"
#include "device.h"

namespace aph
{
VulkanCommandPool::VulkanCommandPool(const CommandPoolCreateInfo& createInfo, VulkanDevice *device, VkCommandPool pool)
    :m_device(device)
{
    getHandle() = pool;
    getCreateInfo() = m_createInfo;
}

VkResult VulkanCommandPool::allocateCommandBuffers(uint32_t commandBufferCount, VkCommandBuffer *pCommandBuffers)
{
    // Safe guard access to internal resources across threads.
    m_spinLock.Lock();

    // Allocate a new command buffer.
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = getHandle(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = commandBufferCount,
    };
    auto result = vkAllocateCommandBuffers(m_device->getHandle(), &allocInfo, pCommandBuffers);

    // Unlock access to internal resources.
    m_spinLock.Unlock();

    // Return result.
    return result;
}
void VulkanCommandPool::freeCommandBuffers(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
    // Safe guard access to internal resources across threads.
    m_spinLock.Lock();
    vkFreeCommandBuffers(m_device->getHandle(), getHandle(), commandBufferCount, pCommandBuffers);
    m_spinLock.Unlock();
}

uint32_t VulkanCommandPool::getQueueFamilyIndex() const
{
    return m_createInfo.queueFamilyIndex;
}
}  // namespace aph