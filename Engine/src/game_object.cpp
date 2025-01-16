#include "game_object.hpp"
#include <stdexcept>

#include "descriptors.hpp"

namespace vke
{
    glm::mat4 TransformComponent::mat4()
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }
    glm::mat3 TransformComponent::normalMatrix()
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.f / scale;

        return glm::mat3{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2),
            },
        };
    }

    void VkeGameObject::initializeDescriptorSet(VkeDevice &device, VkeDescriptorSetLayout &layout, VkeDescriptorPool &globalDescriptorPool, VkeBuffer &uboBuffer)
    {
        auto writer = VkeDescriptorWriter(layout, globalDescriptorPool);
        auto bufferInfo = uboBuffer.descriptorInfo();
        writer.writeBuffer(0, &bufferInfo);
        if (material)
        {
            VkDescriptorImageInfo albedoInfo = material->albedo->getDescriptor();
            VkDescriptorImageInfo normalInfo = material->normal->getDescriptor();
            VkDescriptorImageInfo roughnessInfo = material->roughness->getDescriptor();
            VkDescriptorImageInfo metallicInfo = material->metallic->getDescriptor();
            VkDescriptorImageInfo aoInfo = material->ao->getDescriptor();
            writer.writeImage(1, &albedoInfo);
            writer.writeImage(1, &normalInfo);
            writer.writeImage(1, &roughnessInfo);
            writer.writeImage(1, &metallicInfo);
            writer.writeImage(1, &aoInfo);
        }
        if (!writer.build(descriptorSet))
        {
            throw std::runtime_error("Failed to build descriptor set for game object.");
        }
    }
}