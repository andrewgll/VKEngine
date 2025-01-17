#pragma once

#include "model.hpp"
#include "texture.hpp"
#include "descriptors.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
// std
#include <memory>
#include <unordered_map>

namespace vke
{
    struct VkeMaterialFlags
    {
        bool hasAlbedo = false;
        bool hasNormal = false;
        bool hasRoughness = false;
        bool hasMetallic = false;
        bool hasAO = false;
    };
    struct VkeMaterial
    {
        std::shared_ptr<VkeTexture> albedo;
        std::shared_ptr<VkeTexture> normal;
        std::shared_ptr<VkeTexture> roughness;
        std::shared_ptr<VkeTexture> metallic;
        std::shared_ptr<VkeTexture> ao;
        VkeMaterialFlags flags;
    };

    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent
    {
        float lightIntensity{1.f};
    };

    class VkeGameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, VkeGameObject>;

        static VkeGameObject createGameObject()
        {
            static id_t currentId = 0;
            return VkeGameObject{currentId++};
        }

        static VkeGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        VkeGameObject(const VkeGameObject &) = delete;
        VkeGameObject &operator=(const VkeGameObject &) = delete;
        VkeGameObject(VkeGameObject &&) = default;
        VkeGameObject &operator=(VkeGameObject &&) = default;
        void initializeDescriptorSet(VkeDevice &device, VkeDescriptorSetLayout &layout, VkeDescriptorPool &globalDescriptorPool, VkeBuffer &uboBuffer);
        glm::mat4 getViewProjectionMatrix();
        const id_t getId()
        {
            return id;
        }

        std::shared_ptr<VkeModel> model{};
        std::shared_ptr<VkeMaterial> material;
        glm::vec3 color{};
        TransformComponent transform{};
        VkDescriptorSet descriptorSet{};
        std::unique_ptr<PointLightComponent>
            pointLight = nullptr;

    private:
        bool isOrthographic = true;
        float orthographicSize = 10.0f;
        float fieldOfView = 45.0f;
        float aspectRatio = 1.0f;
        float intensity;

        float nearPlane;
        float farPlane;
        VkeGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
} // namespace vke
