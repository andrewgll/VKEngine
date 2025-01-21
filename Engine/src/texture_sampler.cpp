#include "texture_sampler.hpp"

#include "device.hpp"

#include <stdexcept>

namespace vke
{
    TextureSampler::TextureSampler(VkeDevice &device, VkSamplerAddressMode addressMode) : vkeDevice(device)
    {
        createTextureSampler(addressMode);
    }

    void TextureSampler::createTextureSampler(VkSamplerAddressMode addressMode)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(vkeDevice.getPhysicalDevice(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        samplerInfo.addressModeU = addressMode;
        samplerInfo.addressModeV = addressMode;
        samplerInfo.addressModeW = addressMode;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.maxAnisotropy = 16.0f;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 100.f;

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        textureSampler = VkSampler{};
        if (vkCreateSampler(vkeDevice.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
} // namespace vke