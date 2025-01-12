#pragma once

#include "device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace vke
{

  class VkeSwapChain
  {
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VkeSwapChain(VkeDevice &deviceRef, VkExtent2D windowExtent);
    VkeSwapChain(VkeDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<VkeSwapChain> previous);
    ~VkeSwapChain();

    VkeSwapChain(const VkeSwapChain &) = delete;
    VkeSwapChain operator=(const VkeSwapChain &) = delete;

    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkFramebuffer getShadowMapFrameBuffer(int index) { return shadowMapFramebuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkRenderPass getShadowRenderPass() { return shadowRenderPass; }
    VkImageView getImageView(int index) { return swapChainImageViews[index]; }
    size_t imageCount() { return swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent; }
    uint32_t width() { return swapChainExtent.width; }
    uint32_t height() { return swapChainExtent.height; }

    float extentAspectRatio()
    {
      return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    bool compareSwapFormats(const VkeSwapChain &other) const
    {
      return swapChainImageFormat == other.swapChainImageFormat && swapChainDepthFormat == other.swapChainDepthFormat;
    }

  private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createShadowMapRenderPass();
    void createShadowMapFramebuffers();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkFramebuffer> shadowMapFramebuffers;
    VkRenderPass renderPass;
    VkRenderPass shadowRenderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    VkImageView shadowDepthImageView;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkeDevice &device;
    VkExtent2D windowExtent;
    VkExtent2D shadowMapExtent;

    VkSwapchainKHR swapChain;
    std::shared_ptr<VkeSwapChain> oldSwapChain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
  };

} // namespace lve