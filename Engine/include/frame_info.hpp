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
        glm::vec3 direction{0.f, 0.f, 0.f};
        alignas(16) glm::vec3 color{1.0f, 1.f, 0.4f};
        float intensity{0.1f};
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
        VkeGameObject::Map &gameObjects;
    };
    inline glm::mat4 getLightViewProjection(DirectionalLight &dirLight, const glm::vec3 &sceneCenter, float sceneRadius)
    {
        float zNear = 0.1f;
        float zFar = sceneRadius * 2.0f;      
        float lightSize = sceneRadius * 2.0f; 
        glm::mat4 depthProjectionMatrix = glm::ortho(-lightSize, lightSize, -lightSize, lightSize, zNear, zFar);
        glm::mat4 depthViewMatrix = glm::lookAt(sceneCenter - dirLight.direction * sceneRadius, sceneCenter, glm::vec3(0, 1, 0));
        glm::mat4 depthModelMatrix = glm::mat4(1.0f);
        return depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
    }

} // namespace vke