#include "object_manager.hpp"

#include "texture.hpp"
#include "model.hpp"
#include "game_object.hpp"

#include "memory"
#include <unordered_map>
#include <stdexcept>

namespace vke
{
    ObjectManager::ObjectManager(VkeDevice &vkeDevice) : vkeDevice(vkeDevice)
    {
    }
    ObjectManager &ObjectManager::addModel(const std::string &filepath)
    {
        currentModel = std::make_unique<VkeModel>(vkeDevice, filepath);
        return *this;
    }
    ObjectManager &ObjectManager::addTexture(const std::string &filepath, TextureType type)
    {
        if (type == TextureType::VKE_TEXTURE_TYPE_ALBEDO)
        {
            currentAlbedo = std::make_unique<VkeTexture>(vkeDevice, filepath);
        }
        else if (type == TextureType::VKE_TEXTURE_TYPE_NORMAL)
        {
            currentNormal = std::make_unique<VkeTexture>(vkeDevice, filepath);
        }
        else if (type == TextureType::VKE_TEXTURE_TYPE_ROUGHNESS)
        {
            currentRoughness = std::make_unique<VkeTexture>(vkeDevice, filepath);
        }
        else if (type == TextureType::VKE_TEXTURE_TYPE_METALLIC)
        {
            currentMetallic = std::make_unique<VkeTexture>(vkeDevice, filepath);
        }
        else if (type == TextureType::VKE_TEXTURE_TYPE_AO)
        {
            currentAO = std::make_unique<VkeTexture>(vkeDevice, filepath);
        }
        textureCount++;
        return *this;
    }
    VkeGameObject ObjectManager::build(glm::vec3 translation, glm::vec3 scale)
    {

        auto material = std::make_unique<VkeMaterial>();
        material->flags.hasAlbedo = currentAlbedo != nullptr;
        material->flags.hasNormal = currentNormal != nullptr;
        material->flags.hasRoughness = currentRoughness != nullptr;
        material->flags.hasMetallic = currentMetallic != nullptr;
        material->flags.hasAO = currentAO != nullptr;

        if (!currentModel)
        {
            throw std::runtime_error("Model must be set before building a game object");
        }
        if (!currentAlbedo)
        {
            currentAlbedo = std::make_unique<VkeTexture>(vkeDevice, defaultTexturePath);
            textureCount++;
        }
        if (!currentNormal)
        {
            currentNormal = std::make_unique<VkeTexture>(vkeDevice, defaultNormalPath);
            textureCount++;
        }
        if (!currentRoughness)
        {
            currentRoughness = std::make_unique<VkeTexture>(vkeDevice, defaultRoughnessPath);
            textureCount++;
        }
        if (!currentMetallic)
        {
            currentMetallic = std::make_unique<VkeTexture>(vkeDevice, defaultMetallicPath);
            textureCount++;
        }
        if (!currentAO)
        {
            currentAO = std::make_unique<VkeTexture>(vkeDevice, defaultAOPath);
            textureCount++;
        }

        auto gameObject = VkeGameObject::createGameObject();
        gameObject.model = std::move(currentModel);
        material->albedo = std::move(currentAlbedo);
        material->normal = std::move(currentNormal);
        material->roughness = std::move(currentRoughness);
        material->metallic = std::move(currentMetallic);
        material->ao = std::move(currentAO);
        gameObject.material = std::move(material);
        gameObject.transform.translation = translation;
        gameObject.transform.scale = scale;

        currentModel = nullptr;
        currentAlbedo = nullptr;
        currentNormal = nullptr;
        currentRoughness = nullptr;
        currentMetallic = nullptr;
        currentAO = nullptr;

        return gameObject;
    }
} // namespace vke