#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

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
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);

            void sierpinski(std::vector<VkeModel::Vertex> &vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);


            VkeWindow vkeWindow{WIDTH, HEIGHT, "PIZDEC"};
            VkeDevice vkeDevice{vkeWindow};
            std::unique_ptr<VkeSwapChain> vkeSwapChain;
            std::unique_ptr<VkePipeline> vkePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<VkeModel> vkeModel;
    };
} // namespace vke