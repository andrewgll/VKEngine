#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "camera.hpp"
#include "render_system.hpp"

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
    }
    App::~App()
    {
    }
    void App::run()
    {
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
                vkeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                vkeRenderer.endSwapChainRenderPass(commandBuffer);
                vkeRenderer.endFrame();
            }
            vkDeviceWaitIdle(vkeDevice.device());
        }
    }

    // temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
    std::unique_ptr<VkeModel> createCubeModel(VkeDevice &device, glm::vec3 offset)
    {
        VkeModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto &v : modelBuilder.vertices)
        {
            v.position += offset;
        }

        modelBuilder.indices = {0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

        return std::make_unique<VkeModel>(device, modelBuilder);
    }
    void App::loadGameObjects()
    {
        std::shared_ptr<VkeModel> cubeModel = createCubeModel(vkeDevice, {0.f, 0.f, 0.f});

        auto cube = VkeGameObject::createGameObject();

        cube.model = cubeModel;
        cube.transform.translation = {.0f, .0f, 2.5f};
        cube.transform.scale = {.5f, .5f, .5f};
        gameObjects.push_back(std::move(cube));
    }
}