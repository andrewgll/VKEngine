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
        // float time{0.f};

        // padding to 16 bytes
        int padding[3] = {0, 0, 0};
    };
    class RenderSystem
    {
    public:
        RenderSystem(VkeDevice &device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> &setLayouts);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;
        void renderGameObjects(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(std::vector<VkDescriptorSetLayout> &setLayouts);
        void createPipeline(VkRenderPass renderPass);

        VkeDevice &vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;
    };
} // namespace vke