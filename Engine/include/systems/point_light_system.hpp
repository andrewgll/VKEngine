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
    class PointLightSystem
    {
    public:

        PointLightSystem(VkeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);  
        void render(FrameInfo& frameInfo);
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        VkeDevice& vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;
    };
} // namespace vke