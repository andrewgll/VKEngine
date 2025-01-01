#pragma once

#include <string>
#include <vector>

#include "device.hpp"

namespace vke
{

    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = delete;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class VkePipeline
    {
    public:
        VkePipeline(
            VkeDevice &device,
            const std::string &vertFilepath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo);
        ~VkePipeline();
        VkePipeline(const VkePipeline &) = delete;
        VkePipeline operator=(const VkePipeline &) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
        static void enableAlphaBlending(PipelineConfigInfo &configInfo);

    private:
        std::vector<char> readFile(const std::string &filePath);
        void createGraphicsPipeline(
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        VkeDevice &vkeDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
} // namespace vke