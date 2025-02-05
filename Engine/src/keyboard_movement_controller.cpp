#include "keyboard_movement_controller.hpp"

namespace vke
{
    void KeyboardMovementController::updateShortcuts(GLFWwindow *window)
    {
        static bool keyPressed = false;

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            if (!keyPressed)
            {
                cursorEnabled = !cursorEnabled;
                if (cursorEnabled)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }

                keyPressed = true;
            }
        }
        else
        {
            keyPressed = false;
        }
    }
    void KeyboardMovementController::moveInPlainXZ(GLFWwindow *window, float dt, VkeGameObject &gameObject)
    {
        if (!cursorEnabled)
        {
            static bool firstMouse = true;
            static double lastX, lastY;

            glfwGetCursorPos(window, &xpos, &ypos);

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            double xoffset = xpos - lastX;
            double yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.05f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            gameObject.transform.rotation.y += static_cast<float>(xoffset) * lookSpeed * dt;
            gameObject.transform.rotation.x += static_cast<float>(yoffset) * lookSpeed * dt;

            gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
            gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
        }

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, 1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
            moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
            moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
            moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
            moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
            moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
            moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            gameObject.transform.translation += glm::normalize(moveDir) * moveSpeed * dt;
        }
    }

} // namespace vke
