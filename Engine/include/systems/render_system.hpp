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
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};   
        glm::mat4 normalMatrix{1.f};  
        bool hasNormalMap{false};     
        int padding[3] = {0, 0, 0};   
        glm::mat4 lightViewProj{1.f}; 
    };
    class RenderSystem
    {
    public:
        RenderSystem(VkeDevice &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> &setLayouts);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;
        void renderGameObjects(FrameInfo &frameInfo, DirectionalLight &dirLight);

    private:
        void createPipelineLayout(std::vector<VkDescriptorSetLayout> &setLayouts);
        void createPipeline(VkRenderPass renderPass);

        VkeDevice &vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;
    };
} // namespace vke