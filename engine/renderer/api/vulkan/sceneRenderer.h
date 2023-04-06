#ifndef VKSCENERENDERER_H_
#define VKSCENERENDERER_H_

#include "api/vulkan/device.h"
#include "renderer.h"
#include "renderer/sceneRenderer.h"
#include "scene/mesh.h"

namespace aph
{
struct SceneInfo
{
    glm::vec4 ambient{ 0.04f };
    uint32_t cameraCount{};
    uint32_t lightCount{};
};

// struct CameraInfo
// {
//     glm::mat4 view {1.0f};
//     glm::mat4 proj {1.0f};
//     glm::vec3 viewPos {1.0f};
// };

// struct LightInfo
// {
//     glm::vec3 color {1.0f};
//     glm::vec3 position {1.0f};
//     glm::vec3 direction {1.0f};
// };

struct VulkanUniformData
{
    VulkanUniformData(std::shared_ptr<SceneNode> node) : m_node{
        std::move(node)
    } {}

    void update() { m_buffer->copyTo(m_object->getData()); }

    VulkanBuffer *m_buffer{};

    std::shared_ptr<SceneNode> m_node{};
    std::shared_ptr<UniformObject> m_object{};
};

class VulkanSceneRenderer : public SceneRenderer
{
public:
    VulkanSceneRenderer(const std::shared_ptr<VulkanRenderer> &renderer);
    ~VulkanSceneRenderer() override = default;

    void loadResources() override;
    void cleanupResources() override;
    void update(float deltaTime) override;
    void recordDrawSceneCommands() override;

private:
    void _initSetLayout();
    void _initSampler();
    void _initRenderData();
    void _initForward();
    void _initPostFx();
    void _loadScene();
    void _drawNode(const std::shared_ptr<SceneNode> &node, VulkanPipeline *pipeline,
                         VulkanCommandBuffer *drawCmd);

private:
    enum SetLayoutIndex
    {
        SET_LAYOUT_SAMP,
        SET_LAYOUT_MATERIAL,
        SET_LAYOUT_SCENE,
        // SET_LAYOUT_OBJECT,
        SET_LAYOUT_POSTFX,
        SET_LAYOUT_MAX,
    };

    enum SamplerIndex
    {
        SAMP_TEXTURE,
        // SAMP_SHADOW,
        // SAMP_POSTFX,
        // SAMP_CUBEMAP,
        SAMP_MAX,
    };

    enum PipelineIndex
    {
        PIPELINE_GRAPHICS_FORWARD,
        // PIPELINE_GRAPHICS_SHADOW,
        PIPELINE_COMPUTE_POSTFX,
        PIPELINE_MAX,
    };

    enum BufferIndex
    {
        BUFFER_SCENE_VERTEX,
        BUFFER_SCENE_INDEX,
        BUFFER_MAX,
    };

    std::array<VulkanBuffer *, BUFFER_MAX> m_buffers;
    std::array<VulkanPipeline *, PIPELINE_MAX> m_pipelines;
    std::array<VulkanDescriptorSetLayout *, SET_LAYOUT_MAX> m_setLayouts;
    std::array<VkSampler, SAMP_MAX> m_samplers;

    VkDescriptorSet m_sceneSet{};
    VkDescriptorSet m_samplerSet{};
    std::vector<VkDescriptorSet> m_materialSets{};

    struct
    {
        std::vector<VulkanImage *> colorImages;
        std::vector<VulkanImage *> depthImages;
    } m_forward;

private:
    std::vector<std::shared_ptr<SceneNode>> m_meshNodeList;

    std::deque<std::shared_ptr<VulkanUniformData>> m_uniformDataList;

    std::vector<VkDescriptorBufferInfo> m_cameraInfos{};
    std::vector<VkDescriptorBufferInfo> m_lightInfos{};
    std::vector<VulkanImage *> m_textures{};

private:
    VulkanDevice *m_pDevice{};
    std::shared_ptr<VulkanRenderer> m_pRenderer{};
};
}  // namespace aph

#endif  // VKSCENERENDERER_H_
