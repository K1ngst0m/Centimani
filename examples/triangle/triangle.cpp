#include "triangle.h"

void triangle::initDerive() {
    loadScene();
    buildCommands();
}

void triangle::drawFrame() {
    // float currentFrame    = glfwGetTime();
    // m_frameData.deltaTime = currentFrame - m_frameData.lastFrame;
    // m_frameData.lastFrame = currentFrame;

    // m_renderer->prepareFrame();
    // m_renderer->submitFrame();
    // updateUniformBuffer();
}

void triangle::updateUniformBuffer() {
    // m_sceneManager->update();
    // m_sceneRender->update();
}

void triangle::loadScene() {
    // scene global argument setup
    {
        m_sceneManager = std::make_shared<vkl::SceneManager>();
        m_sceneManager->setAmbient(glm::vec4(0.2f));
    }

    // scene camera
   {
        m_defaultCamera = m_sceneManager->createCamera(m_windowData->getAspectRatio());
        m_defaultCamera->setType(vkl::CameraType::FIRSTPERSON);
        m_defaultCamera->setPosition({0.0f, 1.0f, 3.0f, 1.0f});
        m_defaultCamera->setPerspective(60.0f, m_windowData->getAspectRatio(), 0.1f, 256.0f);
        m_defaultCamera->setMovementSpeed(2.5f);
        m_defaultCamera->setRotationSpeed(0.1f);

        auto &node = m_sceneManager->getRootNode()->createChildNode();
        node->attachObject(m_defaultCamera);

        m_sceneManager->setMainCamera(m_defaultCamera);
    }

    // point light
    {
        m_pointLight = m_sceneManager->createLight();
        m_pointLight->setPosition({1.2f, 1.0f, 2.0f, 1.0f});
        m_pointLight->setDiffuse({0.5f, 0.5f, 0.5f, 1.0f});
        m_pointLight->setSpecular({1.0f, 1.0f, 1.0f, 1.0f});
        m_pointLight->setType(vkl::LightType::POINT);

        auto &node = m_sceneManager->getRootNode()->createChildNode();
        node->attachObject(m_pointLight);
    }

    // direction light
    {
        m_directionalLight = m_sceneManager->createLight();
        m_directionalLight->setDirection({-0.2f, -1.0f, -0.3f, 1.0f});
        m_directionalLight->setDiffuse({0.5f, 0.5f, 0.5f, 1.0f});
        m_directionalLight->setSpecular({1.0f, 1.0f, 1.0f, 1.0f});
        m_directionalLight->setType(vkl::LightType::DIRECTIONAL);

        auto &node = m_sceneManager->getRootNode()->createChildNode();
        node->attachObject(m_directionalLight);
    }

    // load from gltf file
    {
        // glm::mat4 modelTransform = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
        // modelTransform           = glm::rotate(modelTransform, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
        // m_model                  = m_sceneManager->createEntity(modelDir / "Sponza/glTF/Sponza.gltf");
        // auto &node               = m_sceneManager->getRootNode()->createChildNode(modelTransform);
        // node->attachObject(m_model);
    }

    // box prefab
    {
        // glm::mat4 modelTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
        // modelTransform           = glm::rotate(modelTransform, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
        // auto prefab_cube_model        = m_sceneManager->getEntityWithId(vkl::PREFAB_ENTITY_BOX);
        // auto &node = m_sceneManager->getRootNode()->createChildNode(modelTransform);
        // node->attachObject(prefab_cube_model);
    }

    // plane prefab
    {
        // glm::mat4 modelTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
        // modelTransform           = glm::rotate(modelTransform, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
        // auto prefab_plane_model        = m_sceneManager->getEntityWithId(vkl::PREFAB_ENTITY_PLANE);
        // auto &node = m_sceneManager->getRootNode()->createChildNode(modelTransform);
        // node->attachObject(prefab_plane_model);
    }

    // sphere
    {
        // glm::mat4 modelTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
        // modelTransform           = glm::rotate(modelTransform, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
        // auto prefab_sphere_model        = m_sceneManager->getEntityWithId(vkl::PREFAB_ENTITY_SPHERE);
        // auto &node = m_sceneManager->getRootNode()->createChildNode(modelTransform);
        // node->attachObject(prefab_sphere_model);
    }

    {
        m_sceneRenderer = m_renderer->getSceneRenderer();
        m_sceneRenderer->setScene(m_sceneManager);
        m_sceneRenderer->setShadingModel(vkl::ShadingModel::UNLIT);
        m_sceneRenderer->loadResources();
    }

    m_deletionQueue.push_function([&]() {
        m_sceneRenderer->cleanupResources();
    });
}

void triangle::buildCommands() {
    m_sceneRenderer->drawScene();
}

int main() {
    triangle app;

    app.vkl::vklApp::init();
    app.vkl::vklApp::run();
    app.vkl::vklApp::finish();
}