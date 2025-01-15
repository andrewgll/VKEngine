#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game_object.hpp"

namespace vke
{
    class LightObject
    {
    public:
        LightObject(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, float nearPlane, float farPlane, float fieldOfView = 45.0f)
            : position(position), direction(glm::normalize(direction)), color(color), intensity(intensity),
              nearPlane(nearPlane), farPlane(farPlane), fieldOfView(fieldOfView) {}

        glm::mat4 getViewProjectionMatrix()
        {
            glm::mat4 lightView = glm::lookAt(
                position,
                position + direction,
                up);

            glm::mat4 lightProjection;

            if (isOrthographic)
            {
                float orthoSize = 10.0f;
                lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
            }
            else
            {
                lightProjection = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
                lightProjection[1][1] *= -1;
            }

            return lightProjection * lightView;
        }

        void setOrthographic(bool enable, float size = 10.0f)
        {
            isOrthographic = enable;
            orthographicSize = size;
        }

        void setPerspective(float fov, float aspect)
        {
            fieldOfView = fov;
            aspectRatio = aspect;
            isOrthographic = false;
        }
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 color;
        
        float intensity;

        float nearPlane;
        float farPlane;

    private:
        bool isOrthographic = true;
        float orthographicSize = 10.0f;
        float fieldOfView = 45.0f;
        float aspectRatio = 1.0f;

        const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    };
}
