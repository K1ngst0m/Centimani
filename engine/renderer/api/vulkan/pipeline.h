#ifndef PIPELINE_H_
#define PIPELINE_H_

#include "device.h"
#include "shader.h"

namespace vkl
{

enum class VertexComponent
{
    POSITION,
    NORMAL,
    UV,
    COLOR,
    TANGENT,
};

enum class VertexFormat
{
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
};

struct VertexInputBuilder
{
    std::vector<VkVertexInputBindingDescription> _vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> _vertexInputAttributeDescriptions;
    VkPipelineVertexInputStateCreateInfo _pipelineVertexInputStateCreateInfo;
    VkPipelineVertexInputStateCreateInfo &getPipelineVertexInputState(const std::vector<VertexComponent> &components);
};

struct GraphicsPipelineCreateInfo
{
    std::vector<VkDynamicState> dynamicStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VertexInputBuilder vertexInputBuilder;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

    std::vector<VulkanDescriptorSetLayout *> setLayouts;
    std::vector<VkPushConstantRange> constants;
    ShaderMapList shaderMapList;

    GraphicsPipelineCreateInfo(const std::vector<VertexComponent> &component, VkExtent2D extent = { 0, 0 })
    {
        vertexInputInfo = vertexInputBuilder.getPipelineVertexInputState(component);

        inputAssembly =
            vkl::init::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

        dynamicStages = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        dynamicState = vkl::init::pipelineDynamicStateCreateInfo(dynamicStages.data(),
                                                                 static_cast<uint32_t>(dynamicStages.size()));

        rasterizer = vkl::init::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
                                                                     VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
        multisampling = vkl::init::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
        colorBlendAttachment = vkl::init::pipelineColorBlendAttachmentState(
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            VK_FALSE);
        depthStencil = vkl::init::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);
    }

    GraphicsPipelineCreateInfo() :
        GraphicsPipelineCreateInfo({ VertexComponent::POSITION, VertexComponent::NORMAL, VertexComponent::UV,
                                     VertexComponent::COLOR, VertexComponent::TANGENT })
    {
    }
};

struct ComputePipelineCreateInfo
{
    std::vector<VulkanDescriptorSetLayout *> setLayouts;
    std::vector<VkPushConstantRange> constants;
    ShaderMapList shaderMapList;
};

class VulkanPipeline : public ResourceHandle<VkPipeline>
{
public:
    static VulkanPipeline *CreateGraphicsPipeline(VulkanDevice *pDevice, const GraphicsPipelineCreateInfo &createInfo,
                                                  VulkanRenderPass *pRenderPass, VkPipelineLayout layout,
                                                  VkPipeline handle);

    static VulkanPipeline *CreateComputePipeline(VulkanDevice *pDevice, const ComputePipelineCreateInfo &createInfo, VkPipelineLayout layout,
                                                 VkPipeline handle);

    VulkanDescriptorSetLayout *getDescriptorSetLayout(uint32_t idx) { return _setLayouts[idx]; }
    VkPipelineLayout getPipelineLayout() { return _layout; }
    VkPipelineBindPoint getBindPoint() { return _bindPoint; }

protected:
    VkPipelineCache _cache = VK_NULL_HANDLE;
    VulkanDevice *_device = nullptr;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    VkPipelineBindPoint _bindPoint;
    std::vector<VkPushConstantRange> _constants;
    std::vector<VulkanDescriptorSetLayout *> _setLayouts;
    ShaderMapList _shaderMapList;
};

}  // namespace vkl

#endif  // PIPELINE_H_
