#pragma once

#include "device.hpp"
#include <vulkan/vulkan.h>

namespace vke
{
    class TextureSampler
    {
    public:
        TextureSampler(VkeDevice &device);

        // Not copyable or movable
        TextureSampler(TextureSampler &&) = delete;
        TextureSampler(const TextureSampler &) = delete;
        TextureSampler &operator=(TextureSampler &&) = delete;
        TextureSampler operator=(const TextureSampler &) = delete;

        VkSampler getSampler() { return textureSampler; }

    private:
        void createTextureSampler();
        VkeDevice &vkeDevice;
        VkSampler textureSampler;
    };
} // namespace vke