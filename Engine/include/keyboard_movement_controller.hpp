#pragma once

#include "game_object.hpp"
#include "window.hpp"

namespace vke
{
    class KeyboardMovementController
    {

    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        // depends on the GLFW...
        void moveInPlainXZ(GLFWwindow *window, float dt, VkeGameObject &gameObject);
        void updateShortcuts(GLFWwindow *window);
        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
        bool cursorEnabled{false};
        double xpos, ypos;
    };
} // namespace vke
