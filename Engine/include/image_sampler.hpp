#pragma once

#include "device.hpp"

namespace vke
{
    class ImageSampler
    {
    public:
        ImageSampler(VkeDevice &device);
        // Not copyable or movable
        ImageSampler(ImageSampler &&) = delete;
        ImageSampler(const ImageSampler &) = delete;
        ImageSampler &operator=(ImageSampler &&) = delete;
        ImageSampler operator=(const ImageSampler &) = delete;

    private:
        void createTextureSampler();
        VkeDevice &vkeDevice;
        VkSampler textureSampler;
    };
} // namespace vke