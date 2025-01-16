#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "window.hpp"
#include <stdexcept>

namespace vke
{
    VkeWindow::VkeWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    VkeWindow::~VkeWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VkeWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }

    void VkeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("falied to create window surface");
        }
    }

    void VkeWindow::frameBufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto vkeWindow = reinterpret_cast<VkeWindow *>(glfwGetWindowUserPointer(window));
        vkeWindow->frameBufferResized = true;
        vkeWindow->width = width;
        vkeWindow->height = height;
    }

}
