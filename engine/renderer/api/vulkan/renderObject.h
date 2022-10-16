#ifndef VULKAN_RENDERABLE_H_
#define VULKAN_RENDERABLE_H_

#include "sceneRenderer.h"

namespace vkl {
class VulkanRenderObject {
public:
    VulkanRenderObject(VulkanDevice *device, Entity *entity);
    ~VulkanRenderObject() = default;

    void loadResouces();
    void cleanupResources();

    void draw(VulkanPipeline * pipeline, VulkanCommandBuffer *drawCmd);

    void     setupMaterial(VulkanDescriptorSetLayout *materialLayout, uint8_t bindingBits);
    uint32_t getSetCount();

    glm::mat4 getTransform() const;
    void      setTransform(glm::mat4 transform);

private:
    void drawNode(VulkanPipeline * pipeline, VulkanCommandBuffer *drawCmd, const std::shared_ptr<Node> &node);
    void loadTextures();
    void loadBuffer();
    TextureGpuData createTexture(uint32_t width, uint32_t height, void * data, uint32_t dataSize);

    VulkanBuffer                *_vertexBuffer;
    VulkanBuffer                *_indexBuffer;
    TextureGpuData               _emptyTexture;
    std::vector<TextureGpuData>  _textures;
    std::vector<MaterialGpuData> _materialGpuDataList;

private:
    VulkanDevice        *_device = nullptr;
    Entity              *_entity;
    glm::mat4            _transform = glm::mat4(1.0f);
};
} // namespace vkl

#endif // VKRENDERABLE_H_
