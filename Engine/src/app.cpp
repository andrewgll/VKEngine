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
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

    App::App()
    {
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

        RenderSystem renderSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass()};
        VkeCamera camera{};

        auto viewerObject = VkeGameObject::createGameObject();
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
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};
                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);    
                uboBuffers[frameIndex]->flush();
                // render
                vkeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo, gameObjects);
                vkeRenderer.endSwapChainRenderPass(commandBuffer);
                vkeRenderer.endFrame();
            }
            vkDeviceWaitIdle(vkeDevice.device());
        }
    }

    void App::loadGameObjects()
    {
        std::shared_ptr<VkeModel> vkeModel = VkeModel::createModelFromFile(vkeDevice, "models/skull.obj");

        auto gameObj = VkeGameObject::createGameObject();

        gameObj.model = vkeModel;
        gameObj.transform.translation = {.0f, .0f, 2.5f};
        gameObj.transform.scale = {.1f, .1f, .1f};
        gameObjects.push_back(std::move(gameObj));
    }
}