#pragma once
#include "device.hpp"
#include <string>

namespace vke
{
    class Texture
    {
    public:
        Texture(VkeDevice &device, const std::string &filepath);
        ~Texture();
        
        Texture(Texture &&) = delete;
        Texture(const Texture &) = delete;
        Texture &operator=(Texture &&) = delete;
        Texture &operator=(const Texture &) = delete;

        VkSampler getSampler() const { return sampler; }
        VkImageView getImageView() const { return imageView; }
        VkImageLayout getImageLayout() const { return imageLayout; }

    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        VkeDevice &vkeDevice;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;
    };
} // namespace vke