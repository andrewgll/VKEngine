#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "camera.hpp"
#include "systems/render_system.hpp"
#include "systems/point_light_system.hpp"
#include "buffer.hpp"

#define MAX_FRAME_TIME 0.1f

// libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <stdexcept>

namespace vke
{

    App::App()
    {
        loadGameObjects();
        globalPool = VkeDescriptorPool::Builder(vkeDevice)
                         .setMaxSets(VkeSwapChain::MAX_FRAMES_IN_FLIGHT * gameObjects.size())
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkeSwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VkeSwapChain::MAX_FRAMES_IN_FLIGHT * gameObjects.size())
                         .build();
    }
    App::~App()
    {
    }
    void App::run()
    {

        std::vector<std::unique_ptr<VkeBuffer>> uboBuffers(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<VkeBuffer>(
                vkeDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = VkeDescriptorSetLayout::Builder(vkeDevice)
                                   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                   .build();
        auto textureSetLayout = VkeDescriptorSetLayout::Builder(vkeDevice)
                                    .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                                    .build();
        std::vector<VkDescriptorSetLayout> setLayouts = {
            globalSetLayout->getDescriptorSetLayout(),
            textureSetLayout->getDescriptorSetLayout()};

        std::vector<VkDescriptorSet> globalDescriptorSets(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VkeDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        for (auto &gameObject : gameObjects)
        {
            if (gameObject.second.texture)
            {

                VkDescriptorImageInfo imageInfo{};
                imageInfo.sampler = gameObject.second.texture->getSampler();
                imageInfo.imageView = gameObject.second.texture->getImageView();
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VkeDescriptorWriter(*textureSetLayout, *globalPool)
                    .writeImage(1, &imageInfo)
                    .build(gameObject.second.descriptorSet);
            }
        }

        RenderSystem renderSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass(), setLayouts};
        PointLightSystem pointLightSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

        VkeCamera camera{};
        auto viewerObject = VkeGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!vkeWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlainXZ(vkeWindow.getGLWFWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vkeRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 50.f);
            if (auto commandBuffer = vkeRenderer.beginFrame())
            {
                int frameIndex = vkeRenderer.getFrameIndex();
                float currentTimeInSeconds = std::chrono::duration<float, std::chrono::seconds::period>(currentTime.time_since_epoch()).count();
                FrameInfo frameInfo{frameIndex, frameTime, currentTimeInSeconds, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                // render
                vkeRenderer.beginSwapChainRenderPass(commandBuffer);

                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                vkeRenderer.endSwapChainRenderPass(commandBuffer);
                vkeRenderer.endFrame();
            }
            vkDeviceWaitIdle(vkeDevice.device());
        }
    }

    void App::loadGameObjects()
    {
        std::shared_ptr<VkeModel> vkeModel = VkeModel::createModelFromFile(vkeDevice, "models/skull.obj");
        auto skull = VkeGameObject::createGameObject();
        skull.model = vkeModel;
        skull.texture = std::make_unique<VkeTexture>(vkeDevice, "textures/skull.jpg");
        skull.transform.translation = {0.f, 0.5f, 0.f};
        skull.transform.scale = {.04f, .04f, .04f};
        gameObjects.emplace(skull.getId(), std::move(skull));

        vkeModel = VkeModel::createModelFromFile(vkeDevice, "models/eye.obj");
        auto eye = VkeGameObject::createGameObject();
        eye.model = vkeModel;
        eye.texture = std::make_unique<VkeTexture>(vkeDevice, "textures/eye.jpg");
        eye.transform.translation = {1.f, 0.5f, 0.f};
        eye.transform.scale = {.04f, .04f, .04f};
        gameObjects.emplace(eye.getId(), std::move(eye));

        vkeModel = VkeModel::createModelFromFile(vkeDevice, "models/quad.obj");
        auto floor = VkeGameObject::createGameObject();
        floor.model = vkeModel;
        floor.texture = std::make_unique<VkeTexture>(vkeDevice, "textures/default.jpg");
        floor.transform.translation = {1.f, 1.f, 1.f};
        
        floor.transform.scale = {10.f, 10.f, 10.f};
        gameObjects.emplace(floor.getId(), std::move(floor));

        // auto pointLight = VkeGameObject::makePointLight(0.2f); // do not reuse point light again
        // gameObjects.emplace(pointLight.getId(), std::move(pointLight));

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f} //
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            auto pointLight = VkeGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotate = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3{rotate * glm::vec4(-1.f, -1.f, -1.f, 1.f)};
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }

}