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
    RenderSystem::RenderSystem(VkeDevice &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> &setLayouts) : vkeDevice{device}
    {
        createPipelineLayout(setLayouts);
        createPipeline(renderPass);
    }
    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
    }

    void RenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> &setLayouts)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
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
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/shader.vert.spv",
            std::string(VKENGINE_ABSOLUTE_PATH) + "Engine/shaders/shader.frag.spv",
            pipelineConfig);
    }

    void RenderSystem::renderGameObjects(FrameInfo &frameInfo, DirectionalLight &dirLight)
    {

        glm::vec3 lightPos = -dirLight.direction * 20.0f;

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), dirLight.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                   glm::rotate(glm::mat4(1.0f), dirLight.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                   glm::rotate(glm::mat4(1.0f), dirLight.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 rotatedDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(dirLight.direction, 0.0f)));

        glm::mat4 lightView = glm::lookAt(
            lightPos,
            lightPos + rotatedDirection,
            glm::vec3(0.0f, 1.0f, 0.0f));

        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float lightFrustumSize = 20.0f;
        glm::mat4 lightProjection = glm::ortho(
            -lightFrustumSize, lightFrustumSize,
            -lightFrustumSize, lightFrustumSize,
            nearPlane, farPlane);

        glm::mat4 lightViewProj = lightProjection * lightView;

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

        for (auto &kv : frameInfo.gameObjects)
        {
            auto &obj = kv.second;
            if (obj.model == nullptr)
            {
                continue;
            }
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();
            // push.time = frameInfo.frameTime;
            push.hasNormalMap = obj.material->flags.hasNormal;
            push.lightViewProj = lightViewProj;
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                1,
                1,
                &obj.descriptorSet,
                0,
                nullptr);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
}