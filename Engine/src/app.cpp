#include "app.hpp"

#include "render_system.hpp"

// libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

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
        while (!vkeWindow.shouldClose())
        {
            glfwPollEvents();
            if (auto commandBuffer = vkeRenderer.beginFrame())
            {
                vkeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects);
                vkeRenderer.endSwapChainRenderPass(commandBuffer);
                vkeRenderer.endFrame();
            }
            vkDeviceWaitIdle(vkeDevice.device());
        }
    }

    void App::loadGameObjects()
    {
        std::vector<VkeModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0, 0.0}},
            {{-0.5f, 0.5f}, {0.0, 1.0f, 0.0}},
            {{0.5f, 0.5f}, {0.0, 0.0, 1.0f}},
        };
        auto vkeModel = std::make_shared<VkeModel>(vkeDevice, vertices);

        std::vector<glm::vec3> colors = {
            {43.0f / 255.0f, 138.0f / 255.0f, 160.0f / 255.0f},
            {151.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f},
            {160.0f / 255.0f, 201.0f / 255.0f, 211.0f / 255.0f},
            {94.0f / 255.0f, 128.0f / 255.0f, 117.0f / 255.0f},
        };

        for (auto &color : colors)
        {
            color = glm::pow(color, glm::vec3{2.2f});
        }
        for (int i = 0; i < 100; i++)
        {
            auto triangle = VkeGameObject::createGameObject();
            triangle.model = vkeModel;
            triangle.transform2d.scale = glm::vec2(.5f) - i * 0.0025f;
            triangle.transform2d.rotation = i * glm::pi<float>() * .0025f;
            triangle.color = colors[i % colors.size()];
            gameObjects.push_back(std::move(triangle));
        }
    }
}