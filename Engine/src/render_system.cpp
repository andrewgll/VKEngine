#include "render_system.hpp"

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
    struct SimplePushConstantData
    {
        glm::mat4 transform{1.f};
        glm::mat4 normalMatrix{1.f};
    };
    RenderSystem::RenderSystem(VkeDevice &device, VkRenderPass renderPass) : vkeDevice{device}
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }
    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
    }

    void RenderSystem::createPipelineLayout()
    {

        VkPushConstantRange pushConstantRange{};

        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(vkeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void RenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline laout");

        PipelineConfigInfo pipelineConfig{};
        VkePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        vkePipeline = std::make_unique<VkePipeline>(
            vkeDevice,
            "Engine/shaders/shader.vert.spv",
            "Engine/shaders/shader.frag.spv",
            pipelineConfig);
    }

    void RenderSystem::renderGameObjects(FrameInfo &frameInfo, std::vector<VkeGameObject> &gameObjects)
    {
        // render
        vkePipeline->bind(frameInfo.commandBuffer);

        auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

        for (auto &obj : gameObjects)
        {

            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
}