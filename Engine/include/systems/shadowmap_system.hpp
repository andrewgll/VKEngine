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
    VkImageView createShadowMapImageView(VkeDevice &device, int shadowMapExtent);
    struct ShadowMapPushConstants
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 lightViewProj{1.f};
    };

    class ShadowMapSystem
    {
    public:
        ShadowMapSystem(
            VkeDevice &device,
            VkRenderPass shadowRenderPass,
            VkDescriptorSetLayout globalSetLayout,
            VkExtent2D shadowMapExtent,
            VkFramebuffer shadowMapFramebuffer);
        ~ShadowMapSystem();

        ShadowMapSystem(const ShadowMapSystem &) = delete;
        ShadowMapSystem &operator=(const ShadowMapSystem &) = delete;

        void renderShadowMaps(FrameInfo &frameInfo);
        

    private:
        void createPipelineLayout(VkDescriptorSetLayout &setLayout);
        void createPipeline(VkRenderPass renderPass);

        VkeDevice &vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;

        // Shadow map specific resources
        VkRenderPass shadowRenderPass;      // Render pass for shadow map rendering
        VkFramebuffer shadowMapFramebuffer; // Framebuffer for shadow maps
        VkExtent2D shadowMapExtent;         // Resolution of the shadow map
    };
} // namespace vke
