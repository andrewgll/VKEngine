#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

//std
#include <memory>  
#include <vector>

namespace vke{
    class App{
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;
            
            App();
            ~App();

            App(const App &) = delete;
            App &operator = (const App &) = delete;
            void run(); 
        private:
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            VkeWindow vkeWindow{WIDTH, HEIGHT, "YOPTA"};
            VkeDevice vkeDevice{vkeWindow};
            VkeSwapChain vkeSwapChain{vkeDevice, vkeWindow.getExtent()};
            std::unique_ptr<VkePipeline> vkePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
    };
} // namespace vke