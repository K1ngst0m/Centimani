#include "sceneRenderer.h"
#include "commandBuffer.h"
#include "commandPool.h"
#include "common/assetManager.h"
#include "descriptorSetLayout.h"
#include "device.h"
#include "framebuffer.h"
#include "imageView.h"
#include "pipeline.h"
#include "renderObject.h"
#include "renderer/api/vulkan/uiRenderer.h"
#include "renderpass.h"
#include "scene/camera.h"
#include "scene/entity.h"
#include "scene/light.h"
#include "scene/sceneNode.h"
#include "swapChain.h"
#include "uniformObject.h"
#include "vkInit.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkanRenderer.h"

namespace vkl {

namespace {

std::unordered_map<ShadingModel, MaterialBindingBits> materialBindingMap{
    {ShadingModel::UNLIT, MATERIAL_BINDING_UNLIT},
    {ShadingModel::DEFAULTLIT, MATERIAL_BINDING_DEFAULTLIT},
    {ShadingModel::PBR, MATERIAL_BINDING_PBR},
};

VulkanPipeline *CreatePostFxPipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass) {
    VulkanPipeline            *pipeline;
    VulkanDescriptorSetLayout *textureLayout = nullptr;

    // off screen texture
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &textureLayout);
    }

    {
        // build Shader
        std::filesystem::path shaderDir = "assets/shaders/glsl/default";
        EffectInfo            effectInfo{};
        effectInfo.setLayouts.push_back(textureLayout);
        effectInfo.shaderMapList[VK_SHADER_STAGE_VERTEX_BIT]   = pDevice->getShaderCache()->getShaders(shaderDir / "postFX.vert.spv");
        effectInfo.shaderMapList[VK_SHADER_STAGE_FRAGMENT_BIT] = pDevice->getShaderCache()->getShaders(shaderDir / "postFX.frag.spv");

        GraphicsPipelineCreateInfo pipelineCreateInfo{};
        std::vector<VkVertexInputBindingDescription> bindingDescs{
            {0, 4 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX},
        };
        std::vector<VkVertexInputAttributeDescription> attrDescs{
            {0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
            {1, 0, VK_FORMAT_R32G32_SFLOAT, 2 * sizeof(float)}
        };
        pipelineCreateInfo.vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescs.size()),
            .pVertexBindingDescriptions = bindingDescs.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size()),
            .pVertexAttributeDescriptions = attrDescs.data(),
        };
        VK_CHECK_RESULT(pDevice->createGraphicsPipeline(&pipelineCreateInfo, &effectInfo, pRenderPass, &pipeline));
    }

    return pipeline;
}

VulkanPipeline *CreateSkyboxPipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass) {
    VulkanPipeline            *pipeline;
    VulkanDescriptorSetLayout *sceneLayout    = nullptr;
    VulkanDescriptorSetLayout *materialLayout = nullptr;

    // scene
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0));
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &sceneLayout);
    }

    // material
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &materialLayout);
    }

    {
        // build Shader
        std::filesystem::path shaderDir = "assets/shaders/glsl/default";
        EffectInfo            effectInfo{};
        effectInfo.setLayouts.push_back(sceneLayout);
        effectInfo.setLayouts.push_back(materialLayout);
        effectInfo.constants.push_back(vkl::init::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0));
        effectInfo.shaderMapList[VK_SHADER_STAGE_VERTEX_BIT]   = pDevice->getShaderCache()->getShaders(shaderDir / "skybox.vert.spv");
        effectInfo.shaderMapList[VK_SHADER_STAGE_FRAGMENT_BIT] = pDevice->getShaderCache()->getShaders(shaderDir / "skybox.frag.spv");

        GraphicsPipelineCreateInfo pipelineCreateInfo;
        VK_CHECK_RESULT(pDevice->createGraphicsPipeline(&pipelineCreateInfo, &effectInfo, pRenderPass, &pipeline));
    }

    return pipeline;
}

VulkanPipeline *CreateUnlitPipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass, SceneInfo &sceneInfo) {
    VulkanPipeline            *pipeline;
    VulkanDescriptorSetLayout *sceneLayout    = nullptr;
    VulkanDescriptorSetLayout *materialLayout = nullptr;

    // scene
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0, sceneInfo.cameraCount));
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &sceneLayout);
    }

    // material
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &materialLayout);
    }

    {
        // build Shader
        std::filesystem::path shaderDir = "assets/shaders/glsl/default";
        EffectInfo            effectInfo{};
        effectInfo.setLayouts.push_back(sceneLayout);
        effectInfo.setLayouts.push_back(materialLayout);
        effectInfo.constants.push_back(vkl::init::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0));
        effectInfo.shaderMapList[VK_SHADER_STAGE_VERTEX_BIT]   = pDevice->getShaderCache()->getShaders(shaderDir / "unlit.vert.spv");
        effectInfo.shaderMapList[VK_SHADER_STAGE_FRAGMENT_BIT] = pDevice->getShaderCache()->getShaders(shaderDir / "unlit.frag.spv");

        GraphicsPipelineCreateInfo pipelineCreateInfo;
        VK_CHECK_RESULT(pDevice->createGraphicsPipeline(&pipelineCreateInfo, &effectInfo, pRenderPass, &pipeline));
    }

    return pipeline;
}

VulkanPipeline *CreateDefaultLitPipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass, SceneInfo &sceneInfo) {
    VulkanPipeline            *pipeline       = nullptr;
    VulkanDescriptorSetLayout *sceneLayout    = nullptr;
    VulkanDescriptorSetLayout *materialLayout = nullptr;

    // scene
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sceneInfo.cameraCount),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, sceneInfo.lightCount),
        };
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &sceneLayout);
    }

    // material
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1),
        };
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &materialLayout);
    }

    {
        EffectInfo            effectInfo{};
        std::filesystem::path shaderDir = "assets/shaders/glsl/default";
        effectInfo.setLayouts.push_back(sceneLayout);
        effectInfo.setLayouts.push_back(materialLayout);
        effectInfo.constants.push_back(vkl::init::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0));
        effectInfo.shaderMapList[VK_SHADER_STAGE_VERTEX_BIT]   = pDevice->getShaderCache()->getShaders(shaderDir / "default_lit.vert.spv");
        effectInfo.shaderMapList[VK_SHADER_STAGE_FRAGMENT_BIT] = pDevice->getShaderCache()->getShaders(shaderDir / "default_lit.frag.spv");

        GraphicsPipelineCreateInfo pipelineCreateInfo;
        VK_CHECK_RESULT(pDevice->createGraphicsPipeline(&pipelineCreateInfo, &effectInfo, pRenderPass, &pipeline));
    }
    return pipeline;
}

VulkanPipeline *CreatePBRPipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass, SceneInfo &sceneInfo) {
    VulkanPipeline            *pipeline       = nullptr;
    VulkanDescriptorSetLayout *sceneLayout    = nullptr;
    VulkanDescriptorSetLayout *objectLayout   = nullptr;
    VulkanDescriptorSetLayout *materialLayout = nullptr;

    // scene
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, sceneInfo.cameraCount),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 2, sceneInfo.lightCount),
        };
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &sceneLayout);
    }

    // object
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0),
        };
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &objectLayout);
    }

    // material
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0), // material info
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4),
            vkl::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 5),
        };
        VkDescriptorSetLayoutCreateInfo createInfo = vkl::init::descriptorSetLayoutCreateInfo(bindings);
        pDevice->createDescriptorSetLayout(&createInfo, &materialLayout);
    }

    {
        EffectInfo            effectInfo{};
        std::filesystem::path shaderDir = AssetManager::GetShaderDir(ShaderAssetType::GLSL) / "default";
        effectInfo.setLayouts.push_back(sceneLayout);
        effectInfo.setLayouts.push_back(objectLayout);
        effectInfo.setLayouts.push_back(materialLayout);
        effectInfo.constants.push_back(vkl::init::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0));
        effectInfo.shaderMapList[VK_SHADER_STAGE_VERTEX_BIT]   = pDevice->getShaderCache()->getShaders(shaderDir / "pbr.vert.spv");
        effectInfo.shaderMapList[VK_SHADER_STAGE_FRAGMENT_BIT] = pDevice->getShaderCache()->getShaders(shaderDir / "pbr.frag.spv");

        GraphicsPipelineCreateInfo pipelineCreateInfo{};
        VK_CHECK_RESULT(pDevice->createGraphicsPipeline(&pipelineCreateInfo, &effectInfo, pRenderPass, &pipeline));
    }
    return pipeline;
}

VulkanPipeline *CreatePipeline(VulkanDevice *pDevice, VulkanRenderPass *pRenderPass, ShadingModel model, SceneInfo &sceneInfo) {
    switch (model) {
    case ShadingModel::UNLIT:
        return CreateUnlitPipeline(pDevice, pRenderPass, sceneInfo);
    case ShadingModel::DEFAULTLIT:
        return CreateDefaultLitPipeline(pDevice, pRenderPass, sceneInfo);
    case ShadingModel::PBR:
        return CreatePBRPipeline(pDevice, pRenderPass, sceneInfo);
    }
    return nullptr;
}

} // namespace

namespace {
std::shared_ptr<Texture> loadCubemapFromFile(const std::string &filename) {
    auto           texture = std::make_shared<Texture>();
    int            width, height, channels;
    unsigned char *imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    uint32_t       imageSize = width * height * 4;
    texture->width           = width;
    texture->height          = height;
    texture->data.resize(imageSize);
    if (channels == 3) {
        unsigned char *rgba = new unsigned char[texture->data.size()];
        unsigned char *rgb  = imageData;
        for (size_t i = 0; i < width * height; ++i) {
            memcpy(rgba, rgb, sizeof(unsigned char) * 3);
            rgba += 4;
            rgb += 3;
        }
        memcpy(texture->data.data(), rgba, imageSize);
    } else {
        memcpy(texture->data.data(), imageData, imageSize);
    }
    return texture;
}
} // namespace

std::unique_ptr<VulkanSceneRenderer> VulkanSceneRenderer::Create(const std::shared_ptr<VulkanRenderer> &renderer) {
    auto instance = std::make_unique<VulkanSceneRenderer>(renderer);
    return instance;
}

VulkanSceneRenderer::VulkanSceneRenderer(const std::shared_ptr<VulkanRenderer> &renderer)
    : m_pDevice(renderer->getDevice()),
      m_pRenderer(renderer) {
}

void VulkanSceneRenderer::loadResources() {
    // _initPostFxResource();
    _loadSceneNodes();
    _initForwardResource();
    _initPostFxResource();
    _initRenderList();
    _initUniformList();
    isSceneLoaded = true;
}

void VulkanSceneRenderer::cleanupResources() {
    for (auto &renderObject : _renderList) {
        renderObject->cleanupResources();
    }

    for (auto &ubo : _uniformList) {
        ubo->cleanupResources();
    }

    if (_forwardPass.pipeline != nullptr) {
        m_pDevice->destroyPipeline(_forwardPass.pipeline);
    }

    if (_postFxPass.pipeline != nullptr) {
        m_pDevice->destroyPipeline(_postFxPass.pipeline);
    }

    for (uint32_t idx = 0; idx < m_pRenderer->getSwapChain()->getImageCount(); idx++){
        m_pDevice->destroyFramebuffers(_postFxPass.framebuffers[idx]);
        m_pDevice->destroyFramebuffers(_forwardPass.framebuffers[idx]);
        m_pDevice->destroyImage(_forwardPass.colorImages[idx]);
        m_pDevice->destroyImageView(_postFxPass.colorImageViews[idx]);
        m_pDevice->destroyImageView(_forwardPass.colorImageViews[idx]);
        m_pDevice->destroyImage(_forwardPass.depthImages[idx]);
        m_pDevice->destroyImageView(_forwardPass.depthImageViews[idx]);
        vkDestroySampler(m_pDevice->getHandle(), _postFxPass.samplers[idx], nullptr);
    }
    m_pDevice->destroyBuffer(_postFxPass.quadVB);
    m_pDevice->destroyBuffer(_sceneInfoUB);
    vkDestroyRenderPass(m_pDevice->getHandle(), _forwardPass.renderPass->getHandle(), nullptr);
    vkDestroyRenderPass(m_pDevice->getHandle(), _postFxPass.renderPass->getHandle(), nullptr);
}

void VulkanSceneRenderer::drawScene() {
    m_pRenderer->prepareFrame();
    VkExtent2D extent{
        .width  = m_pRenderer->getWindowWidth(),
        .height = m_pRenderer->getWindowHeight(),
    };
    VkViewport viewport = vkl::init::viewport(extent);
    VkRect2D   scissor  = vkl::init::rect2D(extent);

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color        = {{0.1f, 0.1f, 0.1f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.pRenderPass       = _forwardPass.renderPass;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = extent;
    renderPassBeginInfo.clearValueCount   = clearValues.size();
    renderPassBeginInfo.pClearValues      = clearValues.data();

    VkCommandBufferBeginInfo beginInfo = vkl::init::commandBufferBeginInfo();

    auto  commandIndex  = m_pRenderer->getCurrentFrameIndex();
    auto *commandBuffer = m_pRenderer->getDefaultCommandBuffer(commandIndex);

    commandBuffer->begin();

    // dynamic state
    commandBuffer->cmdSetViewport(&viewport);
    commandBuffer->cmdSetSissor(&scissor);
    commandBuffer->cmdBindPipeline(_forwardPass.pipeline);
    commandBuffer->cmdBindDescriptorSet(_forwardPass.pipeline, 0, 1, &_sceneSets[commandIndex]);

    // forward pass
    renderPassBeginInfo.pFramebuffer = _forwardPass.framebuffers[m_pRenderer->getCurrentImageIndex()];
    commandBuffer->cmdBeginRenderPass(&renderPassBeginInfo);
    for (auto &renderable : _renderList) {
        renderable->draw(_forwardPass.pipeline, commandBuffer);
    }
    commandBuffer->cmdEndRenderPass();

    renderPassBeginInfo.pFramebuffer = _postFxPass.framebuffers[m_pRenderer->getCurrentImageIndex()];
    renderPassBeginInfo.pRenderPass = _postFxPass.renderPass;
    renderPassBeginInfo.clearValueCount = 1;
    commandBuffer->cmdBeginRenderPass(&renderPassBeginInfo);
    VkDeviceSize offsets[1] = {0};
    commandBuffer->cmdBindVertexBuffers(0, 1, _postFxPass.quadVB, offsets);
    commandBuffer->cmdBindPipeline(_postFxPass.pipeline);
    commandBuffer->cmdBindDescriptorSet(_postFxPass.pipeline, 0, 1, &_postFxPass.sets[m_pRenderer->getCurrentImageIndex()]);
    commandBuffer->cmdDraw(6, 1, 0, 0);
    commandBuffer->cmdEndRenderPass();

    commandBuffer->end();
    m_pRenderer->submitAndPresent();
}

void VulkanSceneRenderer::update(float deltaTime) {
    for (auto &ubo : _uniformList) {
        ubo->updateBuffer(ubo->getData());
    }
}

void VulkanSceneRenderer::_initRenderList() {
    for (auto &renderable : _renderList) {
        renderable->loadResouces();
    }
}

void VulkanSceneRenderer::_initUniformList() {
    _sceneSets.resize(m_pRenderer->getCommandBufferCount());

    for (auto &set : _sceneSets) {
        set = _forwardPass.pipeline->getDescriptorSetLayout(SET_SCENE)->allocateSet();

        BufferCreateInfo bufferCI{
            .size      = sizeof(SceneInfo),
            .alignment = 0,
            .usage     = BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .property  = MEMORY_PROPERTY_HOST_COHERENT_BIT | MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        };

        VK_CHECK_RESULT(m_pDevice->createBuffer(&bufferCI, &_sceneInfoUB, &_sceneInfo));
        _sceneInfoUB->setupDescriptor();

        std::vector<VkWriteDescriptorSet> writes{
            vkl::init::writeDescriptorSet(set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &_sceneInfoUB->getBufferInfo()),
            vkl::init::writeDescriptorSet(set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, _cameraInfos.data(), _cameraInfos.size()),
        };

        if (getShadingModel() != ShadingModel::UNLIT) {
            writes.push_back(vkl::init::writeDescriptorSet(set, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, _lightInfos.data(), _lightInfos.size()));
        }

        vkUpdateDescriptorSets(m_pDevice->getHandle(), writes.size(), writes.data(), 0, nullptr);
    }

    for (auto &renderable : _renderList) {
        renderable->setupDescriptor(_forwardPass.pipeline->getDescriptorSetLayout(SET_OBJECT),
                                    _forwardPass.pipeline->getDescriptorSetLayout(SET_MATERIAL),
                                    materialBindingMap[getShadingModel()]);
    }
}

void VulkanSceneRenderer::_loadSceneNodes() {
    _sceneInfo.ambient = glm::vec4(_scene->getAmbient(), 1.0f);
    std::queue<std::shared_ptr<SceneNode>> q;
    q.push(_scene->getRootNode());

    while (!q.empty()) {
        auto node = q.front();
        q.pop();

        switch (node->getAttachType()) {
        case AttachType::ENTITY: {
            auto renderable = std::make_shared<VulkanRenderData>(m_pDevice, node);
            _renderList.push_back(renderable);
        } break;
        case AttachType::CAMERA: {
            auto ubo = std::make_shared<VulkanUniformData>(m_pDevice, node);
            _uniformList.push_front(ubo);
            _cameraInfos.push_back(ubo->getBufferInfo());
            _sceneInfo.cameraCount++;
        } break;
        case AttachType::LIGHT: {
            auto ubo = std::make_shared<VulkanUniformData>(m_pDevice, node);
            _uniformList.push_back(ubo);
            _lightInfos.push_back(ubo->getBufferInfo());
            _sceneInfo.lightCount++;
        } break;
        default:
            assert("unattached scene node.");
            break;
        }

        for (const auto &subNode : node->getChildNode()) {
            q.push(subNode);
        }
    }
}

void VulkanSceneRenderer::_initSkyboxResource() {
    VulkanPipeline *pipeline = _skyboxResource.pipeline;
    VkDescriptorSet set      = _skyboxResource.set;
    VulkanImage    *cubemap  = _skyboxResource.cubeMap;

    {
        auto             texture       = loadCubemapFromFile("");
        VulkanBuffer    *stagingBuffer = nullptr;
        BufferCreateInfo bufferCI{
            .size      = static_cast<uint32_t>(texture->data.size()),
            .alignment = 0,
            .usage     = BUFFER_USAGE_TRANSFER_SRC_BIT,
            .property  = MEMORY_PROPERTY_HOST_VISIBLE_BIT | MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };
        VK_CHECK_RESULT(m_pDevice->createBuffer(&bufferCI, &stagingBuffer));
        stagingBuffer->map();
        stagingBuffer->copyTo(texture->data.data(), texture->data.size());
        stagingBuffer->unmap();

        ImageCreateInfo imageCI{
            .extent      = {texture->width, texture->height, 1},
            .flags       = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageType   = IMAGE_TYPE_2D,
            .arrayLayers = 6,
            .usage       = IMAGE_USAGE_SAMPLED_BIT | IMAGE_USAGE_TRANSFER_DST_BIT,
            .property    = MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .format      = FORMAT_B8G8R8A8_UNORM,
            .tiling      = IMAGE_TILING_OPTIMAL,
        };

        VK_CHECK_RESULT(m_pDevice->createImage(&imageCI, &cubemap));

        auto *cmd = m_pDevice->beginSingleTimeCommands(VK_QUEUE_TRANSFER_BIT);
        cmd->cmdTransitionImageLayout(cubemap, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmd->cmdCopyBufferToImage(stagingBuffer, cubemap);
        cmd->cmdTransitionImageLayout(cubemap, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_pDevice->endSingleTimeCommands(cmd);

        ImageViewCreateInfo imageViewCI{
            .viewType  = IMAGE_VIEW_TYPE_CUBE,
            .dimension = IMAGE_VIEW_DIMENSION_CUBE,
            .format    = imageCI.format,
        };

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel            = 0;
        subresourceRange.levelCount              = cubemap->getMipLevels();
        subresourceRange.layerCount              = 6;

        _skyboxResource.cubeMapDescInfo = {
            .sampler     = _skyboxResource.cubeMapSampler,
            .imageView   = _skyboxResource.cubeMapView->getHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
    }

    pipeline = CreateSkyboxPipeline(m_pDevice, m_pRenderer->getDefaultRenderPass());
    set      = pipeline->getDescriptorSetLayout(0)->allocateSet();

    std::vector<VkWriteDescriptorSet> writeDescriptorSets{
        // Binding 0 : Vertex shader uniform buffer
        vkl::init::writeDescriptorSet(
            set,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            0,
            _cameraInfos.data()),
        // Binding 1 : Fragment shader cubemap sampler
        vkl::init::writeDescriptorSet(
            set,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1,
            &_skyboxResource.cubeMapDescInfo)};

    vkUpdateDescriptorSets(m_pDevice->getHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

void VulkanSceneRenderer::_initPostFxResource() {
    uint32_t imageCount = m_pRenderer->getSwapChain()->getImageCount();
    _postFxPass.colorImages.resize(imageCount);
    _postFxPass.colorImageViews.resize(imageCount);
    _postFxPass.framebuffers.resize(imageCount);
    _postFxPass.sets.resize(imageCount);
    _postFxPass.samplers.resize(imageCount);


    // buffer
    {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };

        BufferCreateInfo bufferCI{
            .size = sizeof(quadVertices),
            .alignment = 0,
            .usage = BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .property = MEMORY_PROPERTY_HOST_VISIBLE_BIT | MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        VK_CHECK_RESULT(m_pDevice->createBuffer(&bufferCI, &_postFxPass.quadVB, quadVertices));
    }

    // color attachment
    for (auto idx = 0; idx < imageCount; idx++) {
        auto &colorImage     = _postFxPass.colorImages[idx];
        auto &colorImageView = _postFxPass.colorImageViews[idx];
        auto &framebuffer     = _postFxPass.framebuffers[idx];

        // get swapchain image
        {
            colorImage = m_pRenderer->getSwapChain()->getImage(idx);
        }

        // get image view
        {
            ImageViewCreateInfo createInfo{};
            createInfo.format   = FORMAT_B8G8R8A8_UNORM;
            createInfo.viewType = IMAGE_VIEW_TYPE_2D;
            m_pDevice->createImageView(&createInfo, &colorImageView, colorImage);
        }

        // sampler
        {
            VkSamplerCreateInfo createInfo = vkl::init::samplerCreateInfo();
            VK_CHECK_RESULT(vkCreateSampler(m_pDevice->getHandle(), &createInfo, nullptr, &_postFxPass.samplers[idx]));
        }

        {
            std::vector<VulkanImageView *> attachments{colorImageView};
            FramebufferCreateInfo          createInfo{};
            createInfo.width  = m_pRenderer->getSwapChain()->getExtent().width;
            createInfo.height = m_pRenderer->getSwapChain()->getExtent().height;
            VK_CHECK_RESULT(m_pDevice->createFramebuffers(&createInfo, &framebuffer, attachments.size(), attachments.data()));
        }
    }

    {
        VkAttachmentDescription colorAttachment{
            .format         = m_pRenderer->getSwapChain()->getImageFormat(),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

        };

        std::vector<VkAttachmentDescription> colorAttachments{
            colorAttachment,
        };

        VK_CHECK_RESULT(m_pDevice->createRenderPass(nullptr, &_postFxPass.renderPass, colorAttachments));
    }

    _postFxPass.pipeline = CreatePostFxPipeline(m_pDevice, _postFxPass.renderPass);
    for (uint32_t idx = 0; idx < imageCount; idx++){
        VkDescriptorSet &set = _postFxPass.sets[idx];
        set = _postFxPass.pipeline->getDescriptorSetLayout(0)->allocateSet();

        VkDescriptorImageInfo imageInfo{
            .sampler = _postFxPass.samplers[idx],
            .imageView = _forwardPass.colorImageViews[idx]->getHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        std::vector<VkWriteDescriptorSet> writes{
            vkl::init::writeDescriptorSet(set, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &imageInfo),
        };

        vkUpdateDescriptorSets(m_pDevice->getHandle(),
                            static_cast<uint32_t>(writes.size()),
                            writes.data(), 0, nullptr);
    }
}

void VulkanSceneRenderer::_initForwardResource() {
    uint32_t   imageCount  = m_pRenderer->getSwapChain()->getImageCount();
    VkExtent2D imageExtent = m_pRenderer->getSwapChain()->getExtent();

    _forwardPass.framebuffers.resize(imageCount);
    _forwardPass.colorImages.resize(imageCount);
    _forwardPass.colorImageViews.resize(imageCount);
    _forwardPass.depthImages.resize(imageCount);
    _forwardPass.depthImageViews.resize(imageCount);

    // color attachment
    for (auto idx = 0; idx < imageCount; idx++) {
        auto &colorImage     = _forwardPass.colorImages[idx];
        auto &colorImageView = _forwardPass.colorImageViews[idx];
        auto &depthImage     = _forwardPass.depthImages[idx];
        auto &depthImageView = _forwardPass.depthImageViews[idx];
        auto &framebuffer     = _forwardPass.framebuffers[idx];

        {
            ImageCreateInfo createInfo{
                .extent = {imageExtent.width, imageExtent.height, 1},
                .imageType = IMAGE_TYPE_2D,
                .usage     = IMAGE_USAGE_COLOR_ATTACHMENT_BIT | IMAGE_USAGE_SAMPLED_BIT,
                .property  = MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .format    = FORMAT_B8G8R8A8_UNORM,
            };
            m_pDevice->createImage(&createInfo, &colorImage);

            // for debug
            // colorImage = m_pRenderer->getSwapChain()->getImage(idx);
        }

        {
            ImageViewCreateInfo createInfo{
                .viewType = IMAGE_VIEW_TYPE_2D,
                .format   = FORMAT_B8G8R8A8_UNORM,
            };
            m_pDevice->createImageView(&createInfo, &colorImageView, colorImage);
        }

        {
            VkFormat depthFormat = m_pDevice->getDepthFormat();
            ImageCreateInfo createInfo{};
            createInfo.extent   = {imageExtent.width, imageExtent.height, 1};
            createInfo.format   = static_cast<Format>(depthFormat);
            createInfo.tiling   = IMAGE_TILING_OPTIMAL;
            createInfo.usage    = IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            createInfo.property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            VK_CHECK_RESULT(m_pDevice->createImage(&createInfo, &depthImage));
        }

        VulkanCommandBuffer *cmd = m_pDevice->beginSingleTimeCommands(VK_QUEUE_TRANSFER_BIT);
        cmd->cmdTransitionImageLayout(depthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        m_pDevice->endSingleTimeCommands(cmd);

        {
            ImageViewCreateInfo createInfo{};
            createInfo.format   = FORMAT_D32_SFLOAT;
            createInfo.viewType = IMAGE_VIEW_TYPE_2D;
            VK_CHECK_RESULT(m_pDevice->createImageView(&createInfo, &depthImageView, depthImage));
        }

        {
            std::vector<VulkanImageView *> attachments{colorImageView, depthImageView};
            FramebufferCreateInfo          createInfo{};
            createInfo.width  = imageExtent.width;
            createInfo.height = imageExtent.height;
            VK_CHECK_RESULT(m_pDevice->createFramebuffers(&createInfo, &framebuffer, attachments.size(), attachments.data()));
        }
    }

    {
        VkAttachmentDescription colorAttachment{
            .format         = m_pRenderer->getSwapChain()->getImageFormat(),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

        };
        VkAttachmentDescription depthAttachment{
            .format         = m_pDevice->getDepthFormat(),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        std::vector<VkAttachmentDescription> colorAttachments{
            colorAttachment,
        };

        VK_CHECK_RESULT(m_pDevice->createRenderPass(nullptr, &_forwardPass.renderPass, colorAttachments, depthAttachment));
    }

    _forwardPass.pipeline = CreatePipeline(m_pDevice, _forwardPass.renderPass, getShadingModel(), _sceneInfo);
}
} // namespace vkl
