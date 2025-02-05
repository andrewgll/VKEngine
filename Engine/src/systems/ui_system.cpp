#include "systems/ui_system.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "settings.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include "descriptors.hpp"
#include "device.hpp"

namespace vke
{
    UISystem::UISystem(VkeWindow &vkeWindow, VkeDevice &vkeDevice, VkeDescriptorPool &globalPool, VkeRenderer &vkeRenderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        CustomizeImGuiColors();
        CustomizeImGuiStyle();
        ImGui_ImplGlfw_InitForVulkan(vkeWindow.getGLWFWindow(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vkeDevice.getInstance();
        init_info.RenderPass = vkeRenderer.getSwapChainRenderPass();
        init_info.PhysicalDevice = vkeDevice.getPhysicalDevice();
        init_info.Device = vkeDevice.device();
        init_info.QueueFamily = vkeDevice.findPhysicalQueueFamilies().graphicsFamily;
        init_info.Queue = vkeDevice.graphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = globalPool.getDescriptorPool();
        init_info.Subpass = 0;
        init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;

        ImGui_ImplVulkan_Init(&init_info);
        ImGui_ImplVulkan_CreateFontsTexture();
    }
    UISystem::~UISystem()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void UISystem::CustomizeImGuiColors()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);

        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.4f, 0.6f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.5f, 0.75f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.6f, 0.85f, 1.0f);

        colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.95f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.4f, 0.4f, 0.45f, 1.0f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.1f, 0.1f, 0.15f, 0.5f);

        colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.55f, 0.6f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.3f, 0.45f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.35f, 0.5f, 1.0f);

        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.2f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4f, 0.4f, 0.45f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6f, 0.6f, 0.65f, 1.0f);

        colors[ImGuiCol_Header] = ImVec4(0.3f, 0.4f, 0.55f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.5f, 0.65f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.6f, 0.75f, 1.0f);
    }

    void UISystem::CustomizeImGuiStyle()
    {
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ItemSpacing = ImVec2(10, 10);
        style.ScrollbarSize = 10.0f;
        style.Alpha = 0.9f;
    }
} // namespace vke
