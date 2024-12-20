#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

namespace vke{
    class App{
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;
            
            void run(); 
        private:
            VkeWindow vkeWindow{WIDTH, HEIGHT, "YOPTA"};
            VkeDevice vkeDevice{vkeWindow};
            VkePipeline vkePipeline{
                vkeDevice, 
                "Engine/shaders/shader.vert.spv", 
                "Engine/shaders/shader.frag.spv", 
                VkePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
} // namespace vke