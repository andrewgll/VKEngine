#pragma once

#include "window.hpp"
#include "device.hpp"
#include "descriptors.hpp"
#include "renderer.hpp"

namespace vke
{

    class UISystem
    {
    public:
        UISystem(VkeWindow &vkeWindow, VkeDevice &vkeDevice, VkeDescriptorPool &globalPool, VkeRenderer &vkeRenderer);
        ~UISystem();

        UISystem(const UISystem &) = delete;
        UISystem &operator=(const UISystem &) = delete;

        void CustomizeImGuiColors();
        void CustomizeImGuiStyle();
    };
} // namespace vke