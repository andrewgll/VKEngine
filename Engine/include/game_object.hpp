#pragma once

#include "model.hpp"

//std
#include <memory>

namespace vke
{
    struct Transform2dComponent{
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c,s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, 0.f}, {0.f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class VkeGameObject{
        public:
        
        using id_t = unsigned int;
        
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

        Transform2dComponent transform2d{};

        private:
        VkeGameObject(id_t objId) : id{objId} {}

        id_t id;

    };
} // namespace vke
