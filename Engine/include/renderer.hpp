#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace vke
{
    class VkeRenderer
    {
    public:
        VkeRenderer(VkeWindow &vkeWindow, VkeDevice &vkeDevice);
        ~VkeRenderer();

        VkeRenderer(const VkeRenderer &) = delete;
        VkeRenderer &operator=(const VkeRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return vkeSwapChain->getRenderPass(); }
        VkRenderPass getShadowMapRenderPass() const { return vkeSwapChain->getShadowRenderPass(); }
        VkFramebuffer getSwapChainFrameBuffer(int index) const { return vkeSwapChain->getFrameBuffer(index); }
        VkFramebuffer getShadowMapFrameBuffer(int index) const { return vkeSwapChain->getShadowMapFrameBuffer(index); }
        float getAspectRatio() const { return vkeSwapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }
        int getFrameIndex() const
        {
            assert(isFrameStarted && "cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        VkeWindow &vkeWindow;
        VkeDevice &vkeDevice;
        std::unique_ptr<VkeSwapChain> vkeSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted = false;
    };
} // namespace vke