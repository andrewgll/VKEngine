#include "systems/point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <stdexcept>
#include <map>
#include <settings.hpp>

namespace vke
{

    struct PointLightPushConstants
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem(VkeDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : vkeDevice{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }
    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(vkeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline laout");

        PipelineConfigInfo pipelineConfig{};
        VkePipeline::defaultPipelineConfigInfo(pipelineConfig);
        VkePipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        vkePipeline = std::make_unique<VkePipeline>(
            vkeDevice,
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/point_light.vert.spv",
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/point_light.frag.spv",
            pipelineConfig);
    }

    void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo)
    {
        // rotation matrix
        auto rotate = glm::rotate(glm::mat4(1.f), frameInfo.frameTime, {0.f, -1.f, 0.f});

        int lightIndex = 0;
        for (auto &lightObject : frameInfo.lightObjects)
        {


            assert(lightIndex < MAX_LIGHTS && "Exceeded max point lights");

            // update point light position
            lightObject.position = glm::vec3(rotate * glm::vec4(lightObject.position, 1.f));

            // copy light to ubo
            ubo.pointLights[lightIndex].position = glm::vec4(lightObject.position, 1.f);
            ubo.pointLights[lightIndex].color = glm::vec4(lightObject.color, lightObject.intensity);

            lightIndex++;
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo &frameInfo)
    {
        // sort lights
        for (auto &lightObject : frameInfo.lightObjects)
        {
            auto offset = frameInfo.camera.getPosition() - lightObject.position;
            float disSquared = glm::dot(offset, offset);
        }

        // render
        vkePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);
        for (auto &lightObject : frameInfo.lightObjects)
        {

            PointLightPushConstants push{};
            push.position = glm::vec4(lightObject.position, 1.f);
            push.color = glm::vec4(lightObject.color, lightObject.intensity);
            push.radius = lightObject.scale.x;
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push);
            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }
}