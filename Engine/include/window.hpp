#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <string>

namespace vke
{
    class VkeWindow
    {

    public:
        VkeWindow(int w, int h, std::string name);
        ~VkeWindow();

        VkeWindow(const VkeWindow &) = delete;
        VkeWindow &operator=(const VkeWindow &) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }

        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        bool wasWindowResized() { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }
        GLFWwindow *getGLWFWindow() const { return window; }

    private:
        static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int width;
        int height;
        bool frameBufferResized = false;

        std::string windowName;
        GLFWwindow *window;
    };
} // namespace vke