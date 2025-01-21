#pragma once

#include "texture.hpp"
#include "model.hpp"
#include "game_object.hpp"
#include "settings.hpp"

#include "memory"
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace vke
{
    typedef enum TextureType
    {
        VKE_TEXTURE_TYPE_ALBEDO,
        VKE_TEXTURE_TYPE_NORMAL,
        VKE_TEXTURE_TYPE_ROUGHNESS,
        VKE_TEXTURE_TYPE_METALLIC,
        VKE_TEXTURE_TYPE_AO
    } TextureType;
    class ObjectManager
    {
    public:
        ObjectManager(VkeDevice &vkeDevice);
        ~ObjectManager() = default;
        // Not copyable or movable
        ObjectManager(ObjectManager &&) = delete;
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(ObjectManager &&) = delete;
        ObjectManager operator=(const ObjectManager &) = delete;

        ObjectManager &addModel(const std::string &filepath);
        ObjectManager &addTexture(const std::string &filepath, TextureType type = TextureType::VKE_TEXTURE_TYPE_ALBEDO);
        VkeGameObject build(glm::vec3 translation = {0.f, 0.f, 0.f}, glm::vec3 scale = {1.f, 1.f, 1.f});

        float getTextureCount() { return textureCount; }

    private:
        VkeDevice &vkeDevice;
        float textureCount{0};
        const std::string defaultTexturePath = std::string(VKENGINE_ABSOLUTE_PATH) + "textures/default_albedo.jpg";
        const std::string defaultNormalPath = std::string(VKENGINE_ABSOLUTE_PATH) + "textures/default_normal.jpg";
        const std::string defaultRoughnessPath = std::string(VKENGINE_ABSOLUTE_PATH) + "textures/default_roughness.jpg";
        const std::string defaultMetallicPath = std::string(VKENGINE_ABSOLUTE_PATH) + "textures/default_metallic.jpg";
        const std::string defaultAOPath = std::string(VKENGINE_ABSOLUTE_PATH) + "textures/default_AO.jpg";

        std::shared_ptr<VkeModel> currentModel;

        std::shared_ptr<VkeTexture> currentAlbedo;
        std::shared_ptr<VkeTexture> currentNormal;
        std::shared_ptr<VkeTexture> currentRoughness;
        std::shared_ptr<VkeTexture> currentMetallic;
        std::shared_ptr<VkeTexture> currentAO;
    };
} // namespace vke