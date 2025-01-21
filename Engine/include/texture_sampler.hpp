#pragma once

#include "device.hpp"
#include <vulkan/vulkan.h>

namespace vke
{
    class TextureSampler
    {
    public:
        TextureSampler(VkeDevice &device, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

        // Not copyable or movable
        TextureSampler(TextureSampler &&) = delete;
        TextureSampler(const TextureSampler &) = delete;
        TextureSampler &operator=(TextureSampler &&) = delete;
        TextureSampler operator=(const TextureSampler &) = delete;

        VkSampler getSampler() { return textureSampler; }

    private:
        void createTextureSampler(VkSamplerAddressMode addressMode);
        VkeDevice &vkeDevice;
        VkSampler textureSampler;
    };
} // namespace vke