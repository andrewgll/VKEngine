#include "model.hpp"

// std
#include <cstring>
#include <cassert>

namespace vke
{
    VkeModel::VkeModel(VkeDevice &device, const VkeModel::Builder &builder) : vkeDevice{device}
    {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }
    VkeModel::~VkeModel()
    {
        vkDestroyBuffer(vkeDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(vkeDevice.device(), vertexBufferMemory, nullptr);
        if (hasIndexBuffer)
        {
            vkDestroyBuffer(vkeDevice.device(), indexBuffer, nullptr);
            vkFreeMemory(vkeDevice.device(), indexBufferMemory, nullptr);
        }
    }
    void VkeModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        // create a staging buffer on GPU
        vkeDevice.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               stagingBuffer,
                               stagingBufferMemory);

        void *data;
        // map staging memory to gpu(from CPU)
        vkMapMemory(vkeDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        // copy data to gpu
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        // unmap staging memory from cpu
        vkUnmapMemory(vkeDevice.device(), stagingBufferMemory);
        // efficient gpu buffer
        vkeDevice.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               vertexBuffer,
                               vertexBufferMemory);
        vkeDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(vkeDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(vkeDevice.device(), stagingBufferMemory, nullptr);
    }
    void VkeModel::createIndexBuffers(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer)
        {
            return;
        }
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        // create a staging buffer on GPU
        vkeDevice.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               stagingBuffer,
                               stagingBufferMemory);

        void *data;
        // map staging memory to gpu(from CPU)
        vkMapMemory(vkeDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        // copy data to gpu
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        // unmap staging memory from cpu
        vkUnmapMemory(vkeDevice.device(), stagingBufferMemory);
        // efficient gpu buffer
        vkeDevice.createBuffer(bufferSize,
                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               indexBuffer,
                               indexBufferMemory);
        vkeDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(vkeDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(vkeDevice.device(), stagingBufferMemory, nullptr);
    }
    void VkeModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }
    void VkeModel::draw(VkCommandBuffer commandBuffer)
    {
        if (hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> VkeModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    std::vector<VkVertexInputAttributeDescription> VkeModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }
}