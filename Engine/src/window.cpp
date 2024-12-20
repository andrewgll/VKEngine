#include "window.hpp"

#include <stdexcept>

namespace vke{
    VkeWindow::VkeWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    VkeWindow::~VkeWindow(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VkeWindow::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void VkeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("falied to create window surface");
        }
    }

}
