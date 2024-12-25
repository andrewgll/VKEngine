#pragma once

#include "window.hpp"
#include "game_object.hpp"
#include "device.hpp"
#include "renderer.hpp"
// std
#include <memory>
#include <vector>

namespace vke
{
    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        void run();

    private:
        void loadGameObjects();


        VkeWindow vkeWindow{WIDTH, HEIGHT, "PIZDEC"};
        VkeDevice vkeDevice{vkeWindow};
        VkeRenderer vkeRenderer{vkeWindow, vkeDevice};
        std::vector<VkeGameObject> gameObjects;
    };
} // namespace vke