#pragma once
#include "device.hpp"

#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vke
{
    class VkeModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        VkeModel(VkeDevice &device, const VkeModel::Builder &builder);
        ~VkeModel();

        VkeModel(const VkeModel &) = delete;
        VkeModel &operator=(const VkeModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);
        
        VkeDevice &vkeDevice;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
}