#ifndef VULKAN_RENDERER_H_
#define VULKAN_RENDERER_H_

#include "renderer/renderer.h"
#include "sceneRenderer.h"

namespace vkl {
struct PerFrameSyncObject {
    VkSemaphore renderSemaphore;
    VkSemaphore presentSemaphore;
    VkFence     inFlightFence;
};

class VulkanRenderer : public Renderer {
public:
    VulkanRenderer()  = default;
    ~VulkanRenderer() = default;

    void init() override;
    void destroyDevice() override;
    void idleDevice() override;

public:
    void prepareFrame();
    void submitFrame();
    void initDefaultResource();
    void initImGui();
    void prepareUIDraw();

public:
    VkQueue              getDefaultDeviceQueue(QueueFlags type) const;
    VkRenderPass         getDefaultRenderPass() const;
    uint32_t             getCommandBufferCount() const;
    VulkanCommandBuffer *getDefaultCommandBuffer(uint32_t idx) const;
    VkFramebuffer        getDefaultFrameBuffer(uint32_t idx) const;
    PipelineBuilder     &getPipelineBuilder();

    std::shared_ptr<VulkanDevice>  getDevice();
    std::shared_ptr<SceneRenderer> getSceneRenderer() override;

private:
    void _createInstance();
    void _createDevice();
    void _createSurface();
    void _setupDebugMessenger();
    void _setupSwapChain();
    void _setupPipelineBuilder();

private:
    void _createDefaultDepthAttachments();
    void _createDefaultColorAttachments();
    void _createDefaultRenderPass();
    void _createDefaultFramebuffers();
    void _createDefaultSyncObjects();
    void _createDefaultCommandBuffers();

private:
    // TODO
    void getEnabledFeatures() {
    }

private:
    std::vector<const char *> getRequiredInstanceExtensions();
    void                      destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
    const PerFrameSyncObject &getCurrentFrameSyncObject();

private:
    VkInstance                    m_instance;
    std::shared_ptr<VulkanDevice> m_device;
    VulkanSwapChain              *m_swapChain;
    std::vector<const char *>     m_supportedInstanceExtensions;
    VkPhysicalDeviceFeatures      m_enabledFeatures{};
    VkDebugUtilsMessengerEXT      m_debugMessenger;
    VkSurfaceKHR                  m_surface;
    PipelineBuilder               m_pipelineBuilder;
    DeletionQueue                 m_deletionQueue;

    uint32_t m_currentFrame = 0;
    uint32_t m_imageIdx     = 0;

private:
    std::vector<VulkanCommandBuffer *> m_defaultCommandBuffers;
    std::vector<PerFrameSyncObject>    m_defaultSyncObjects;

    struct FrameBufferData {
        VulkanImage       *colorImage;
        VulkanImageView   *colorImageView;
        VulkanImage       *depthImage;
        VulkanImageView   *depthImageView;
        VulkanFramebuffer *framebuffer;
    };
    std::vector<FrameBufferData> m_defaultFramebuffers;
    VulkanRenderPass            *m_defaultRenderPass;
};
} // namespace vkl

#endif // VULKANRENDERER_H_
