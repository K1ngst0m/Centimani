#ifndef VULKAN_DEVICE_H_
#define VULKAN_DEVICE_H_

#include "physicalDevice.h"
#include "renderer/device.h"
#include "renderer/gpuResource.h"
#include "vkInit.hpp"
#include "vkUtils.h"
#include "buffer.h"
#include "commandBuffer.h"
#include "commandPool.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"
#include "framebuffer.h"
#include "image.h"
#include "imageView.h"
#include "pipeline.h"
#include "queue.h"
#include "renderpass.h"
#include "shader.h"
#include "swapChain.h"
#include "syncPrimitivesPool.h"
#include "vkInit.hpp"
#include "vkUtils.h"

namespace vkl
{


enum DeviceCreateFlagBits
{

};
using DeviceCreateFlags = uint32_t;

struct DeviceCreateInfo
{
    DeviceCreateFlags flags;
    std::vector<const char *> enabledExtensions;
    VulkanPhysicalDevice *pPhysicalDevice = nullptr;
};

class VulkanDevice : public GraphicsDevice, public ResourceHandle<VkDevice>
{
private:
    VulkanDevice() = default;
public:
    static VkResult Create(const DeviceCreateInfo &createInfo, VulkanDevice **ppDevice);

    static void Destroy(VulkanDevice *pDevice);

public:
    VkResult createBuffer(const BufferCreateInfo &createInfo, VulkanBuffer **ppBuffer, void *data = nullptr);

    VkResult createImage(const ImageCreateInfo &createInfo, VulkanImage **ppImage);

    VkResult createImageView(const ImageViewCreateInfo &createInfo, VulkanImageView **ppImageView, VulkanImage *pImage);

    VkResult createFramebuffers(FramebufferCreateInfo *pCreateInfo, VulkanFramebuffer **ppFramebuffer,
                                uint32_t attachmentCount, VulkanImageView **pAttachments);

    VkResult createRenderPass(const RenderPassCreateInfo &createInfo, VulkanRenderPass **ppRenderPass);

    VkResult createSwapchain(const SwapChainCreateInfo& createInfo, VulkanSwapChain **ppSwapchain);

    VkResult createCommandPool(VulkanCommandPool **ppPool, uint32_t queueFamilyIndex,
                               VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VkResult createGraphicsPipeline(const GraphicsPipelineCreateInfo &createInfo, VulkanRenderPass *pRenderPass,
                                    VulkanPipeline **ppPipeline);

    VkResult createComputePipeline(const ComputePipelineCreateInfo &createInfo, VulkanPipeline **ppPipeline);

    VkResult createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo *pCreateInfo,
                                       VulkanDescriptorSetLayout **ppDescriptorSetLayout);

public:
    void destroyBuffer(VulkanBuffer *pBuffer);
    void destroyImage(VulkanImage *pImage);
    void destroyImageView(VulkanImageView *pImageView);
    void destroyFramebuffers(VulkanFramebuffer *pFramebuffer);
    void destoryRenderPass(VulkanRenderPass *pRenderpass);
    void destroySwapchain(VulkanSwapChain *pSwapchain);
    void destroyCommandPool(VulkanCommandPool *pPool);
    void destroyPipeline(VulkanPipeline *pipeline);
    void destroyDescriptorSetLayout(VulkanDescriptorSetLayout *pLayout);

public:
    VkResult allocateCommandBuffers(uint32_t commandBufferCount, VulkanCommandBuffer **ppCommandBuffers, VulkanQueue *pQueue);

    void freeCommandBuffers(uint32_t commandBufferCount, VulkanCommandBuffer **ppCommandBuffers);

    VulkanCommandBuffer *beginSingleTimeCommands(VulkanQueue * pQueue);

    void endSingleTimeCommands(VulkanCommandBuffer *commandBuffer);

    void waitIdle();

public:
    VulkanSyncPrimitivesPool *getSyncPrimitiviesPool() { return m_syncPrimitivesPool; }
    VulkanShaderCache *getShaderCache() { return m_shaderCache; }
    VulkanCommandPool *getCommandPoolWithQueue(VulkanQueue *queue);
    VulkanPhysicalDevice *getPhysicalDevice() const;
    VulkanQueue *getQueueByFlags(QueueTypeFlags flags, uint32_t queueIndex = 0);
    VkFormat getDepthFormat() const;

private:
    VulkanPhysicalDevice *m_physicalDevice;
    VkPhysicalDeviceFeatures m_enabledFeatures;

    std::vector<QueueFamily> m_queues = {};

    QueueFamilyCommandPools m_commandPools;

    VulkanSyncPrimitivesPool *m_syncPrimitivesPool = nullptr;
    VulkanShaderCache *m_shaderCache = nullptr;

    DeviceCreateInfo m_createInfo;
};

}  // namespace vkl

#endif  // VKLDEVICE_H_
