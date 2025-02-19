#pragma once

#include "camera.hpp"
#include "pipeline.hpp"
#include "game_object.hpp"
#include "device.hpp"
#include "frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace vke
{
    struct ShadowMapPushConstants
    {
        glm::mat4 modelMatrix{1.f};
    };

    class ShadowMapSystem
    {
    public:
        ShadowMapSystem(
            VkeDevice &device,
            VkRenderPass shadowRenderPass,
            VkDescriptorSetLayout globalSetLayout,
            VkExtent2D shadowMapExtent);
        ~ShadowMapSystem();

        ShadowMapSystem(const ShadowMapSystem &) = delete;
        ShadowMapSystem &operator=(const ShadowMapSystem &) = delete;

        void renderShadowMaps(FrameInfo &frameInfo, glm::mat4 &lightViewProj);
        static glm::mat4 getLightViewProjection(const glm::vec3 &dirLightPos, const glm::vec3 &cameraPosition, float sceneRadius, VkeCamera &camera);

    private:
        void createPipelineLayout(VkDescriptorSetLayout &setLayout);
        void createPipeline(VkRenderPass renderPass);
        VkImageView createShadowMapImageView(VkeDevice &device, int shadowMapExtent);
        void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask);
        VkeDevice &vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;

        // Shadow map specific resources
        VkRenderPass shadowRenderPass; // Render pass for shadow map rendering
        VkExtent2D shadowMapExtent;    // Resolution of the shadow map
    };
} // namespace vke
