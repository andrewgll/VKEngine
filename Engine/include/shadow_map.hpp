#pragma once

#include "device.hpp"
#include "texture_sampler.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
namespace vke
{
    class ShadowMap
    {
    public:
        ShadowMap(VkeDevice &device, uint32_t width, uint32_t height);
        ~ShadowMap();

        VkImageView getImageView() const { return imageView; }
        VkSampler getSampler() const { return sampler; }
        VkFramebuffer getFramebuffer() const { return framebuffer; }
        VkRenderPass getRenderPass() const { return renderPass; }

        glm::mat4 getLightSpaceMatrix(const glm::vec3 &lightDir, const glm::vec3 &sceneCenter, float sceneRadius);

    private:
        void createDepthResources(uint32_t width, uint32_t height);
        void createRenderPass();
        void createFramebuffer();

        float depthBiasConstant = 1.25f;
        float depthBiasSlope = 1.75f;

        glm::vec3 lightPos = glm::vec3();
        float lightFOV = 45.0f;

        VkeDevice &device;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView imageView;
        VkSampler sampler;

        VkRenderPass renderPass;
        VkFramebuffer framebuffer;
    };

}