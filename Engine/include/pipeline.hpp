#pragma once

#include <string>
#include <vector>

#include "device.hpp"

namespace vke {

    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
 

    class VkePipeline{
        public:
            VkePipeline(
                VkeDevice& device, 
                const std::string& vertFilepath,
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo);
            ~VkePipeline();
            VkePipeline(const VkePipeline&) = delete;
            void operator = (const VkePipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);

            std::vector<char> readFile(const std::string& filePath);
            static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        private:
            void createGraphicsPipeline(
                const std::string& vertFilePath, 
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo);
            
            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            VkeDevice& vkeDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;

    };
} // namespace vke