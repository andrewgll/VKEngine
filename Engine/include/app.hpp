#pragma once

#include "window.hpp"
#include "game_object.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "object_manager.hpp"
#include "light_object.hpp"
#include "frame_info.hpp"

// std
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace vke
{
    class App
    {
    public:
        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        void run();

        ObjectManager objectManager{vkeDevice};

    private:
        VkDescriptorSet createDescriptorSet(VkeTexture &texture);
        void loadGameObjects();
        void loadLights();
        void createDescriptors();
        void createUBOBuffers();
        VkeWindow vkeWindow{WIDTH,
                            HEIGHT,
                            "VKEngine v2"};
        VkeDevice vkeDevice{vkeWindow};
        VkeRenderer vkeRenderer{vkeWindow, vkeDevice};

        std::unique_ptr<VkeDescriptorPool> globalPool{};
        VkeGameObject::Map gameObjects;

        std::vector<std::unique_ptr<VkeBuffer>> uboBuffers;
        std::vector<std::unique_ptr<VkeBuffer>> shadowUboBuffers;
        std::unique_ptr<VkeDescriptorSetLayout> globalSetLayout;
        std::unique_ptr<VkeDescriptorSetLayout> shadowSetLayout;
        std::unique_ptr<VkeDescriptorSetLayout> materialSetLayout;

        std::vector<VkDescriptorSet> globalDescriptorSets;
        std::vector<VkDescriptorSet> shadowDescriptorSets;
    };
} // namespace vke