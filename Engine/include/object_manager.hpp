#pragma once

#include "texture.hpp"
#include "model.hpp"
#include "game_object.hpp"

#include "memory"
#include <unordered_map>

namespace vke
{
    class ObjectManager
    {
    public:
        ObjectManager(VkeDevice &vkeDevice, const std::string &defaultTexturePath);
        ~ObjectManager() = default;
        // Not copyable or movable
        ObjectManager(ObjectManager &&) = delete;
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(ObjectManager &&) = delete;
        ObjectManager operator=(const ObjectManager &) = delete;

        ObjectManager &addModel(const std::string &filepath);
        ObjectManager &addTexture(const std::string &filepath);
        VkeGameObject build(glm::vec3 translation = {0.f, 0.f, 0.f}, glm::vec3 scale = {1.f, 1.f, 1.f});

    private:
        VkeDevice &vkeDevice;

        const std::string defaultTexturePath;

        std::shared_ptr<VkeModel> currentModel;
        std::shared_ptr<VkeTexture> currentTexture;
    };
} // namespace vke