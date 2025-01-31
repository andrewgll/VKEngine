#pragma once

#include "camera.hpp"
#include "game_object.hpp"
#include "light_object.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vke
{
#define MAX_LIGHTS 10

    struct PointLight
    {
        glm::vec4 position{};
        glm::vec4 color{};
    };
    struct DirectionalLight
    {
        glm::mat4 lightViewProj{1};
        // x z y
        glm::vec3 direction{1.f, 1.f, 2.f};
        alignas(16) glm::vec3 color{1.0f, 1.f, 0.4f};
        float intensity{1.f};
    };
    struct ShadowUbo {
        glm::mat4 lightViewProj{1};
    };
    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLight{1.f, 1.f, 1.f, .03f};
        PointLight pointLights[MAX_LIGHTS];
        alignas(16) DirectionalLight dirLight;
        int numLights;
    };
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        float currentTimeInSeconds;
        VkCommandBuffer commandBuffer;
        VkeCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        VkDescriptorSet shadowDescriptorSet;
        VkeGameObject::Map &gameObjects;
    };
   

} // namespace vke