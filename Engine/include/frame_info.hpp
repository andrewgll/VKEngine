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
        glm::vec3 direction{-1.f, -1.f, 0.f}; // Direction towards the light source
        float padding1;
        glm::vec3 color{0.f, 1.f, 0.f}; // RGB color of the light
        float intensity{255.f};         // Intensity of the light
    };
    // global uniform buffer object
    // like a push constant, but for uniform buffers
    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};                  // to transform value from camera to world space
        glm::vec4 ambientLight{1.f, 1.f, 1.f, .03f}; // w is intensity
        PointLight pointLights[MAX_LIGHTS];
        DirectionalLight dirLight;
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
        VkeGameObject::Map &gameObjects;
        std::vector<LightObject> lightObjects;
    };
} // namespace vke