#pragma once

#include "model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

namespace vke
{
    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class VkeGameObject{
        public:
        
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, VkeGameObject>;
        
        static VkeGameObject createGameObject() {
            static id_t currentId = 0;
            return VkeGameObject{currentId++};
        }

        VkeGameObject(const VkeGameObject&) = delete;
        VkeGameObject &operator = (const VkeGameObject&) =delete;
        VkeGameObject(VkeGameObject&&) = default;
        VkeGameObject &operator = (VkeGameObject&&) = default;

        const id_t getId() {
            return id;
        }

        std::shared_ptr<VkeModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

        private:
        VkeGameObject(id_t objId) : id{objId} {}

        id_t id;

    };
} // namespace vke
