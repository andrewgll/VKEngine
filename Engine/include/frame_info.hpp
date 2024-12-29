#pragma once

#include "camera.hpp"

// libs
#include "vulkan/vulkan.h"

namespace vke
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VkeCamera &camera;
    };
} // namespace vke