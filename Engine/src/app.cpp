#include "app.hpp"

//std
#include <stdexcept>

namespace vke{
    App::App(){
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }
    App::~App(){
        vkDestroyPipelineLayout(vkeDevice.device(), pipelineLayout, nullptr);
     }
    void App::run() {
        while(!vkeWindow.shouldClose()){
            glfwPollEvents();
        }
    }

    void App::createPipelineLayout(){
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(vkeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void App::createPipeline(){
        auto pipelineConfig = VkePipeline::defaultPipelineConfigInfo(vkeSwapChain.width(), vkeSwapChain.height());
        pipelineConfig.renderPass = vkeSwapChain.getRenderPass(); 
        pipelineConfig.pipelineLayout = pipelineLayout;
        vkePipeline = std::make_unique<VkePipeline>(
            vkeDevice,
            "Engine/shaders/shader.vert.spv",
            "Engine/shaders/shader.frag.spv",
            pipelineConfig
        );
    }

    void App::createCommandBuffers() {}
    void App::drawFrame() {}
}