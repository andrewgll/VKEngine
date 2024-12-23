#include "app.hpp"

//std
#include <stdexcept>
#include <array>

namespace vke{
    App::App(){
        loadModels();
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
            drawFrame();
        }
        vkDeviceWaitIdle(vkeDevice.device());
    }
    
    void App::sierpinski(std::vector<VkeModel::Vertex> &vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top){
        if (depth <= 0) {
            vertices.push_back({top});
            vertices.push_back({right});
            vertices.push_back({left});
        } else {
            auto leftTop = 0.5f * (left + top);
            auto rightTop = 0.5f * (right + top);
            auto leftRight = 0.5f * (left + right);
            sierpinski(vertices, depth - 1, left, leftRight, leftTop);
            sierpinski(vertices, depth - 1, leftRight, right, rightTop);
            sierpinski(vertices, depth - 1, leftTop, rightTop, top);
        }
    }

    void App::loadModels() {
        // std::vector<VkeModel::Vertex> vertices{
        //     {{0.0f, -0.5f}},
        //     {{0.5f, 0.5f}},
        //     {{-0.5f, 0.5f}},
        // };
        // std::vector<VkeModel::Vertex> vertices{};
        // sierpinski(vertices, 3, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, -0.5f});
        std::vector<VkeModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0, 0.0}},
            {{-0.5f, 0.5f}, {0.0, 1.0f, 0.0}},
            {{0.5f, 0.5f}, {0.0, 0.0, 1.0f}},
        };
        vkeModel = std::make_unique<VkeModel>(vkeDevice, vertices);
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

    void App::createCommandBuffers() {
        commandBuffers.resize(vkeSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(vkeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffer");
        }

         for(int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
                throw std::runtime_error("failed to begin rendering command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = vkeSwapChain.getRenderPass();
            renderPassInfo.framebuffer = vkeSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0,0};
            renderPassInfo.renderArea.extent = vkeSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkePipeline->bind(commandBuffers[i]);
            vkeModel->bind(commandBuffers[i]);
            vkeModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to record command buffer");
            }
         }
    }
    void App::drawFrame() {
        uint32_t imageIndex;
        auto result = vkeSwapChain.acquireNextImage(&imageIndex);

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("falied to acquire swap chain image");
        }
        result = vkeSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image");
        }

    }
}