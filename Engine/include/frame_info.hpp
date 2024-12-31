#pragma once

#include "camera.hpp"
#include "game_object.hpp"

// libs
#include "vulkan/vulkan.h"

namespace vke
{
    #define MAX_LIGHTS 10

    struct PointLight{
        glm::vec4 position{};
        glm::vec4 color{};
    };
    // global uniform buffer object
    // like a push constant, but for uniform buffers
    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambientLight{1.f, 1.f, 1.f, .02f}; // w is intensity
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
    };
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VkeCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        VkeGameObject::Map &gameObjects;
    };
} // namespace vke