#include "model.hpp"

#include "utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cstring>
#include <cassert>
#include <iostream>
#include <unordered_map>

namespace std
{
    // hash function for Vertex to check for duplicates
    template <>
    struct hash<vke::VkeModel::Vertex>
    {
        size_t operator()(vke::VkeModel::Vertex const &vertex) const
        {
            size_t seed = 0;
            lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

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

    std::unique_ptr<VkeModel> VkeModel::createModelFromFile(VkeDevice &device, const std::string &filepath)
    {
        Builder builder{};
        builder.loadModels(filepath);
        //todo: only for debug
        std::cout << "Model loaded from file: " << filepath << std::endl;
        std::cout << "Vertex count:" << builder.vertices.size() << std::endl;
        return std::make_unique<VkeModel>(device, builder);
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
    void VkeModel::Builder::loadModels(const std::string &filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};
                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};
                    auto colorIndex = 3 * index.vertex_index + 2;
                    if (colorIndex < attrib.colors.size())
                    {
                        vertex.color = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0]};
                    }
                    else
                    {
                        vertex.color = {1.f, 1.f, 1.f}; // default color
                    }
                }
                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                }
                if (index.texcoord_index >= 0)
                {
                    vertex.uv = {
                        attrib.texcoords[3 * index.texcoord_index + 0],
                        attrib.texcoords[3 * index.texcoord_index + 1],
                    };
                }

                if(uniqueVertices.count(vertex) == 0){
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}