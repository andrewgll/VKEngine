#include "app.hpp"

namespace vke{
    void App::run() {
        while(!vkeWindow.shouldClose()){
            glfwPollEvents();
        }
    }
}