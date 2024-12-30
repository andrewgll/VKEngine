#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "camera.hpp"
#include "render_system.hpp"
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
    // global uniform buffer object
    // like a push constant, but for uniform buffers
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLight{1.f, 1.f, 1.f, .02f}; // w is intensity
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{-1.f}; // w is intensity
    };

    App::App()
    {
        globalPool = VkeDescriptorPool::Builder(vkeDevice)
                         .setMaxSets(VkeSwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkeSwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
        loadGameObjects();
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

        std::vector<VkDescriptorSet> globalDescriptorSets{VkeSwapChain::MAX_FRAMES_IN_FLIGHT};
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VkeDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        RenderSystem renderSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
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
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                // render
                vkeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo);
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
        skull.transform.translation = {0.f, .5f, 0.f};
        skull.transform.scale = {.01f, .01f, .01f};
        gameObjects.emplace(skull.getId(), std::move(skull));

        vkeModel = VkeModel::createModelFromFile(vkeDevice, "models/quad.obj");
        auto floor = VkeGameObject::createGameObject();
        floor.model = vkeModel;
        floor.transform.translation = {0.f, .5f, 0.f};
        floor.transform.scale = {.5f, .1f, .5f};
        gameObjects.emplace(floor.getId(), std::move(floor));
    }
}