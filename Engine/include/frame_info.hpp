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
        glm::vec3 direction{0.f, 1.f, 1.f};
        alignas(16) glm::vec3 color{1.0f, 1.f, 0.4f};
        float intensity{1.f};
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
    inline glm::mat4 getLightViewProjection(DirectionalLight &dirLight, const glm::vec3 &cameraPosition, float sceneRadius)
    {
        float zNear = 0.1f;
        float zFar = sceneRadius * 1.5f;      
        float lightSize = sceneRadius * 1.5f;
        glm::vec3 lightTarget = cameraPosition;                                   
        glm::vec3 lightPosition = lightTarget - dirLight.direction * sceneRadius; 

        glm::mat4 depthProjectionMatrix = glm::ortho(-lightSize, lightSize, -lightSize, lightSize, zNear, zFar);
        depthProjectionMatrix[1][1] *= -1.0f;
        glm::mat4 depthViewMatrix = glm::lookAt(lightPosition, lightTarget, glm::vec3(0, 1, 0));
        return depthProjectionMatrix * depthViewMatrix;
    }

} // namespace vke