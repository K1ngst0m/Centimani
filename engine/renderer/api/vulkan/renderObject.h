#ifndef VULKAN_RENDERABLE_H_
#define VULKAN_RENDERABLE_H_

#include "device.h"

namespace vkl {
class VulkanSceneRenderer;
class Entity;
class ShaderPass;
struct Vertex;
struct SubEntity;
class VulkanImage;
class VulkanImageView;
class VulkanSampler;

struct MaterialGpuData {
    VkDescriptorSet set;
    VkPipeline pipeline;
};

struct TextureGpuData {
    VulkanImage     *image = nullptr;
    VulkanImageView *imageView = nullptr;
    VkSampler        sampler = VK_NULL_HANDLE;

    VkDescriptorImageInfo descriptorInfo;
};

enum MaterialBindingBits {
    MATERIAL_BINDING_NONE      = (1 << 0),
    MATERIAL_BINDING_BASECOLOR = (1 << 1),
    MATERIAL_BINDING_NORMAL    = (1 << 2),
};

class VulkanRenderObject {
public:
    VulkanRenderObject(VulkanSceneRenderer *renderer, VulkanDevice* device, Entity *entity);
    ~VulkanRenderObject() = default;

    void loadResouces();
    void cleanupResources();

    void draw(VkPipelineLayout layout, VulkanCommandBuffer *drawCmd);

    void     setupMaterial(VkDescriptorSetLayout *materialLayout, VkDescriptorPool descriptorPool, uint8_t bindingBits);
    uint32_t getSetCount();

    glm::mat4 getTransform() const;
    void      setTransform(glm::mat4 transform);

private:
    void drawNode(VkPipelineLayout layout, VulkanCommandBuffer *drawCmd, const std::shared_ptr<SubEntity> &node);
    void loadTextures();
    void loadBuffer();
    void createEmptyTexture();

    VulkanBuffer                *_vertexBuffer;
    VulkanBuffer                *_indexBuffer;
    TextureGpuData               _emptyTexture;
    std::vector<TextureGpuData>  _textures;
    std::vector<MaterialGpuData> _materialGpuDataList;

private:
    VulkanDevice        *_device = nullptr;
    VulkanSceneRenderer *_sceneRenderer;
    Entity              *_entity;
    glm::mat4            _transform = glm::mat4(1.0f);
};
} // namespace vkl

#endif // VKRENDERABLE_H_
