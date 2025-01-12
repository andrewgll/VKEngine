#include "systems/shadowmap_system.hpp"
#include "systems/render_system.hpp"

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

    ShadowMapSystem::ShadowMapSystem(
        VkeDevice &device,
        VkRenderPass shadowRenderPass,
        VkDescriptorSetLayout globalSetLayout,
        VkExtent2D shadowMapExtent,
        VkFramebuffer shadowMapFramebuffer) : vkeDevice{device},
                                              shadowRenderPass{shadowRenderPass},
                                              shadowMapExtent{shadowMapExtent},
                                              shadowMapFramebuffer{shadowMapFramebuffer}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(shadowRenderPass);
    }
    ShadowMapSystem::~ShadowMapSystem()
    {
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
    }
    VkImageView createShadowMapImageView(VkeDevice &device, int shadowMapExtent)
    {
        VkImage shadowMapImage;
        VkDeviceMemory shadowMapMemory;
        VkImageView shadowMapImageView;

        // Create the shadow map image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = shadowMapExtent;
        imageInfo.extent.height = shadowMapExtent;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_D32_SFLOAT; // Depth format
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateImage(device.device(), &imageInfo, nullptr, &shadowMapImage);

        // Allocate memory and bind it
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device.device(), shadowMapImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(device.device(), &allocInfo, nullptr, &shadowMapMemory);
        vkBindImageMemory(device.device(), shadowMapImage, shadowMapMemory, 0);

        // Create the image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = shadowMapImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D32_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(device.device(), &viewInfo, nullptr, &shadowMapImageView);
        return shadowMapImageView;
    }
    void ShadowMapSystem::createPipelineLayout(VkDescriptorSetLayout &setLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &setLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(vkeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void ShadowMapSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        VkePipeline::defaultPipelineConfigInfo(pipelineConfig);
        VkePipeline::defaultShadowPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;

        pipelineConfig.pipelineLayout = pipelineLayout;
        vkePipeline = std::make_unique<VkePipeline>(
            vkeDevice,
            "Engine/shaders/shadow.vert.spv",
            // "Engine/shaders/shadow.frag.spv",
            pipelineConfig);
    }

    void ShadowMapSystem::renderShadowMaps(FrameInfo &frameInfo)
    {
        for (auto &light : frameInfo.lights)
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = shadowRenderPass;
            renderPassInfo.framebuffer = shadowMapFramebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = shadowMapExtent;
            VkClearValue clearValue{};
            clearValue.depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearValue;

            vkCmdBeginRenderPass(frameInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkePipeline->bind(frameInfo.commandBuffer);

            for (auto &kv : frameInfo.gameObjects)
            {
                auto &obj = kv.second;
                if (!obj.model)
                    continue;

                ShadowMapPushConstants push{};
                push.modelMatrix = obj.transform.mat4();
                push.lightViewProj = light.getViewProjectionMatrix();

                vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(ShadowMapPushConstants),
                    &push);

                obj.model->bind(frameInfo.commandBuffer);
                obj.model->draw(frameInfo.commandBuffer);
            }

            vkCmdEndRenderPass(frameInfo.commandBuffer);
        }
    }
}