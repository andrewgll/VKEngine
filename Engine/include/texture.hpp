#pragma once

#include "device.hpp"

// libs
#include "vulkan/vulkan.h"

// std
#include <string>

namespace vke
{
    class VkeTexture
    {
    public:
        VkeTexture(VkeDevice &device, const std::string &filename);
        ~VkeTexture();
        // Not copyable or movable
        VkeTexture(VkeTexture &&) = delete;
        VkeTexture(const VkeTexture &) = delete;
        VkeTexture &operator=(VkeTexture &&) = delete;
        VkeTexture operator=(const VkeTexture &) = delete;

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }

    private:
        void loadTexture(const std::string &filename);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void createTextureImage(const std::string &filename);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        VkImageView createImageView(VkImage image, VkFormat format, VkeDevice &device);

        VkImage image;
        VkFormat imageFormat;
        VkSampler sampler;
        VkeDevice &vkeDevice;
        VkImageView imageView;
        VkImageLayout imageLayout;
        VkDeviceMemory imageMemory;

        VkImageCreateInfo createInfo{};
        VkImageViewCreateInfo viewInfo{};

        int texWidth{0};
        int texHeight{0};
        int texChannels{0};
        int mipLevels{0};
    };
    ;
} // namespace vke