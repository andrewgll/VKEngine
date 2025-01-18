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
#include <settings.hpp>

namespace vke
{

    ShadowMapSystem::ShadowMapSystem(
        VkeDevice &device,
        VkRenderPass shadowRenderPass,
        VkDescriptorSetLayout globalSetLayout,
        VkExtent2D shadowMapExtent) : vkeDevice{device},
                                      shadowRenderPass{shadowRenderPass},
                                      shadowMapExtent{shadowMapExtent}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(shadowRenderPass);
    }
    ShadowMapSystem::~ShadowMapSystem()
    {
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
    }
    void ShadowMapSystem::createPipelineLayout(VkDescriptorSetLayout &setLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ShadowMapPushConstants);

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
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/shadow.vert.spv",
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/shadow.frag.spv",
            pipelineConfig);
    }

    void ShadowMapSystem::renderShadowMaps(FrameInfo &frameInfo, DirectionalLight &dirLight)
    {
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float lightSize = 50.0f; 

        
        glm::vec3 lightPosition = dirLight.direction * -1.0f; 
        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        
        glm::mat4 lightProjection = glm::ortho(-lightSize, lightSize, -lightSize, lightSize, nearPlane, farPlane);

        
        glm::mat4 lightViewProj = lightProjection * lightView;


        vkePipeline->bind(frameInfo.commandBuffer);

        for (auto &kv : frameInfo.gameObjects)
        {
            auto &obj = kv.second;
            if (!obj.model)
                continue;
            ShadowMapPushConstants push{};
            push.modelMatrix = obj.transform.mat4();
            push.lightViewProj = lightProjection * lightView;
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
        }
}