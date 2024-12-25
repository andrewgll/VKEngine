#pragma once

#include "pipeline.hpp"
#include "game_object.hpp"
#include "device.hpp"
// std
#include <memory>
#include <vector>

namespace vke
{
    class RenderSystem
    {
    public:

        RenderSystem(VkeDevice& device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;
        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VkeGameObject>& gameObjects);
    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        VkeDevice& vkeDevice;
        std::unique_ptr<VkePipeline> vkePipeline;
        VkPipelineLayout pipelineLayout;
    };
} // namespace vke