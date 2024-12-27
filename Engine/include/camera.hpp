#pragma once

// libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vke {

class VkeCamera{
    public:
        void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);

        void setPerspectiveProjection(float fov, float aspectRatio, float near, float far);

        const glm::mat4 getProjection() const { return projectionMatrix; }

    private:
    glm::mat4 projectionMatrix{1.f};
};

} // namespace vke