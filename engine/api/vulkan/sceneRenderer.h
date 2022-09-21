#ifndef VKSCENERENDERER_H_
#define VKSCENERENDERER_H_

#include "renderObject.h"
#include "uniformBufferObject.h"

namespace vkl {

class VulkanSceneRenderer : public SceneRenderer {
public:
    VulkanSceneRenderer(SceneManager *scene, VkCommandBuffer commandBuffer, vkl::Device *device, VkQueue graphics, VkQueue transfer);
    void loadResources() override;
    void cleanupResources() override;
    void update() override;
    void drawScene() override;

private:
    void _initRenderList();
    void _initUboList();
    void _loadSceneNodes(SceneNode *node);

private:
    vkl::Device     *_device;
    VkCommandBuffer  _drawCmd;
    VkDescriptorPool _descriptorPool;
    VkQueue          _transferQueue;
    VkQueue          _graphicsQueue;

private:
    std::vector<VulkanRenderObject *>       _renderList;
    std::deque<VulkanUniformBufferObject *> _uboList;

    VulkanUniformBufferObject *cameraUBO = nullptr;
};
} // namespace vkl

#endif // VKSCENERENDERER_H_
