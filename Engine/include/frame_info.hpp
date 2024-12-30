#pragma once

#include "camera.hpp"
#include "game_object.hpp"

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
        VkDescriptorSet globalDescriptorSet;
        VkeGameObject::Map &gameObjects;
    };
} // namespace vke