#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vke {
    class VkeWindow{

        public:
            VkeWindow(int w, int h, std::string name);
            ~VkeWindow();

            VkeWindow(const VkeWindow &) = delete;
            VkeWindow &operator = (const VkeWindow &) = delete;

            bool shouldClose() {return glfwWindowShouldClose(window);}

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private:

            void initWindow();

            const int width;
            const int height;

            std::string windowName;
            GLFWwindow *window;

    };
} // namespace vke