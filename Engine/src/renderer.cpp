#include "renderer.hpp"

#include "app.hpp"

// std
#include <stdexcept>
#include <array>

namespace vke
{
    VkeRenderer::VkeRenderer(VkeWindow &window, VkeDevice &device) : vkeWindow(window), vkeDevice(device)
    {
        recreateSwapChain();
        createCommandBuffers();
    }
    VkeRenderer::~VkeRenderer()
    {
        freeCommandBuffers();
    }

    void VkeRenderer::createCommandBuffers()
    {
        commandBuffers.resize(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vkeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vkeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffer");
        }
    }
    void VkeRenderer::recreateSwapChain()
    {
        auto extent = vkeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = vkeWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(vkeDevice.device());
        VkExtent2D shadowExtent = {SHADOWMAP_DIM, SHADOWMAP_DIM};

        if (vkeSwapChain == nullptr)
        {
            vkeSwapChain = std::make_unique<VkeSwapChain>(vkeDevice, extent, shadowExtent);
        }
        else
        {
            std::shared_ptr<VkeSwapChain> oldSwapChain = std::move(vkeSwapChain);
            vkeSwapChain = std::make_unique<VkeSwapChain>(vkeDevice, extent, shadowExtent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*vkeSwapChain))
            {
                throw std::runtime_error("Swap chain image or depth format has changed");
            }
        }
    }
    void VkeRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(vkeDevice.device(), vkeDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer VkeRenderer::beginFrame()
    {
        assert(!isFrameInProgress() && "cannot call beginFrame while frame is in progress");
        auto result = vkeSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("falied to acquire swap chain image");
        }
        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin rendering command buffer");
        }
        return commandBuffer;
    }
    void VkeRenderer::endFrame()
    {
        assert(isFrameInProgress() && "cannot call endFrame while frame not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }
        auto result = vkeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkeWindow.wasWindowResized())
        {
            vkeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image. Error code: " + std::to_string(result));
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VkeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void VkeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass if frame not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "cannot begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = vkeSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vkeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vkeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vkeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vkeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void VkeRenderer::beginShadowSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        std::array<VkClearValue, 2> clearValues{};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkeSwapChain->getShadowRenderPass();
        renderPassInfo.framebuffer = vkeSwapChain->getShadowMapFrameBuffer();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vkeSwapChain->getShadowMapExtent();

        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vkeSwapChain->getShadowMapExtent().width);
        viewport.height = static_cast<float>(vkeSwapChain->getShadowMapExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vkeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void VkeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "cannot end render pass when frame not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "cannot end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}