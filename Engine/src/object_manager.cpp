#include "object_manager.hpp"

#include "texture.hpp"
#include "model.hpp"
#include "game_object.hpp"

#include "memory"
#include <unordered_map>

namespace vke
{
    ObjectManager::ObjectManager(VkeDevice &vkeDevice, const std::string &defaultTexturePath) : vkeDevice{vkeDevice}, defaultTexturePath{defaultTexturePath}
    {

    }
    ObjectManager &ObjectManager::addModel(const std::string &filepath)
    {
        currentModel = std::make_unique<VkeModel>(vkeDevice, filepath);
        return *this;
    }
    ObjectManager &ObjectManager::addTexture(const std::string &filepath)
    {
        currentTexture = std::make_unique<VkeTexture>(vkeDevice, filepath);
        return *this;
    }
    VkeGameObject ObjectManager::build(glm::vec3 translation, glm::vec3 scale)
    {
        if (!currentModel)
        {
            throw std::runtime_error("Model must be set before building a game object");
        }
        if (!currentTexture)
        {
            currentTexture = std::make_unique<VkeTexture>(vkeDevice, "textures/default.jpg");
        }

        auto gameObject = VkeGameObject::createGameObject();
        gameObject.model = std::move(currentModel);
        gameObject.texture = std::move(currentTexture);
        gameObject.transform.translation = translation;
        gameObject.transform.scale = scale;

        currentModel = nullptr;
        currentTexture = nullptr;

        return gameObject;
    }
} // namespace vke