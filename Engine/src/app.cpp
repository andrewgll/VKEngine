#include "app.hpp"

//libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <stdexcept>
#include <array>

namespace vke{
    struct SimplePushConstantData{
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };
    App::App(){
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
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
        if(vkCreatePipelineLayout(vkeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void App::createPipeline(){
        assert(vkeSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline laout");

        PipelineConfigInfo pipelineConfig {};
        VkePipeline::defaultPipelineConfigInfo(pipelineConfig );
        pipelineConfig.renderPass = vkeSwapChain->getRenderPass(); 
        pipelineConfig.pipelineLayout = pipelineLayout;
        vkePipeline = std::make_unique<VkePipeline>(
            vkeDevice,
            "Engine/shaders/shader.vert.spv",
            "Engine/shaders/shader.frag.spv",
            pipelineConfig
        );
    }

    void App::createCommandBuffers() {
        commandBuffers.resize(vkeSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(vkeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffer");
        }

         
    }

    void App::freeCommandBuffers() {
        vkFreeCommandBuffers(vkeDevice.device(), vkeDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }
    void App::recordCommandBuffer(int imageIndex){
        static int frame = 30;
        frame = (frame + 1) % 100;

        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("failed to begin rendering command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = vkeSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = vkeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vkeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vkeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vkeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        vkePipeline->bind(commandBuffers[imageIndex]);
        vkeModel->bind(commandBuffers[imageIndex]);

        for(int j = 0; j < 4; j++){
            SimplePushConstantData push{};
            push.offset = {-0.5f + frame * 0.002f, -0.4f + j * 0.25f};
            push.color = {0.0f, 0.0f, 0.2f + 0.2f *j};

            vkCmdPushConstants(commandBuffers[imageIndex], 
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                0, 
                sizeof(SimplePushConstantData), 
                &push);
            vkeModel->draw(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void App::drawFrame() {
        uint32_t imageIndex;
        auto result = vkeSwapChain->acquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return;
        }

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("falied to acquire swap chain image");
        }
        recordCommandBuffer(imageIndex);
        result = vkeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkeWindow.wasWindowResized()){
            vkeWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image");
        }

    }

    void App::recreateSwapChain() {
        vkDeviceWaitIdle(vkeDevice.device());
        auto extent = vkeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = vkeWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(vkeDevice.device());


        if(vkeSwapChain == nullptr){
            vkeSwapChain = std::make_unique<VkeSwapChain>(vkeDevice, extent);
        }
        else{
            vkeSwapChain = std::make_unique<VkeSwapChain>(vkeDevice, extent, std::move(vkeSwapChain));
            if(vkeSwapChain->imageCount() != commandBuffers.size()){
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        createPipeline();
    }

}