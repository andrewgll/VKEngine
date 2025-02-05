#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "camera.hpp"
#include "systems/render_system.hpp"
#include "systems/point_light_system.hpp"
#include "systems/shadowmap_system.hpp"
#include "systems/ui_system.hpp"
#include "buffer.hpp"
#include "object_manager.hpp"
#include "light_object.hpp"
#include "texture_sampler.hpp"

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

// libs
#define GLM_FORCE_RADIANT
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <stdexcept>
#include <iostream>

namespace vke
{

    App::App()
    {
        loadGameObjects();
        loadLights();
        // global pool must be created first
        globalPool = VkeDescriptorPool::Builder(vkeDevice)
                         .setMaxSets(MAX_FRAMES_IN_FLIGHT * gameObjects.size() * 2 * 2)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * gameObjects.size() * 2 * 2)         // Increase if needed
                         .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT * gameObjects.size() * 2 * 2) // Increase if needed
                         .build();
        createUBOBuffers();
        createDescriptors();
    }
    App::~App()
    {
    }
    void App::run()
    {
        glfwSetInputMode(vkeWindow.getGLWFWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        std::vector<VkDescriptorSetLayout> setLayouts = {
            globalSetLayout->getDescriptorSetLayout(),
            materialSetLayout->getDescriptorSetLayout()};

        UISystem uiSystem{vkeWindow, vkeDevice, *globalPool, vkeRenderer};
        RenderSystem renderSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass(), setLayouts};
        PointLightSystem pointLightSystem{vkeDevice, vkeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        ShadowMapSystem shadowMapSystem{vkeDevice, vkeRenderer.getShadowMapRenderPass(), shadowSetLayout->getDescriptorSetLayout(), {SHADOWMAP_DIM, SHADOWMAP_DIM}};

        VkeCamera camera{};
        auto viewerObject = VkeGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        // setup before render

        auto sun = VkeGameObject::makePointLight(0.3f);
        sun.color = glm::vec3(1, 1, 0.5);
        sun.transform.translation = glm::vec3(1.f, 2.f, 2.f);
        gameObjects.emplace(sun.getId(), std::move(sun));
        auto cameraOffset = glm::vec3(-10.f, 10.f, -2.f);

        while (!vkeWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlainXZ(vkeWindow.getGLWFWindow(), frameTime, viewerObject);
            cameraController.updateShortcuts(vkeWindow.getGLWFWindow());
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vkeRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 50.f);

            if (auto commandBuffer = vkeRenderer.beginFrame())
            {
                int frameIndex = vkeRenderer.getFrameIndex();
                float currentTimeInSeconds = std::chrono::duration<float, std::chrono::seconds::period>(currentTime.time_since_epoch()).count();
                FrameInfo frameInfo{frameIndex,
                                    frameTime,
                                    currentTimeInSeconds,
                                    commandBuffer,
                                    camera,
                                    globalDescriptorSets[frameIndex],
                                    shadowDescriptorSets[frameIndex],
                                    gameObjects};

                // update
                GlobalUbo ubo{};
                ShadowUbo shadowUbo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();

                glm::mat4 lightViewProj = ShadowMapSystem::getLightViewProjection(sun.transform.translation, frameInfo.camera.getPosition() + cameraOffset, 10.f, camera);
                ubo.dirLight.lightViewProj = lightViewProj;
                ubo.dirLight.color = sun.color;
                ubo.dirLight.direction = sun.transform.translation;
                shadowUbo.lightViewProj = lightViewProj;

                pointLightSystem.update(frameInfo, ubo);

                shadowUboBuffers[frameIndex]->writeToBuffer(&shadowUbo);
                shadowUboBuffers[frameIndex]->flush();

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                vkeRenderer.beginShadowSwapChainRenderPass(commandBuffer);
                shadowMapSystem.renderShadowMaps(frameInfo, lightViewProj);
                vkeRenderer.endSwapChainRenderPass(commandBuffer);

                vkeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                renderImGuiFrame(commandBuffer, sun, cameraOffset);
                vkeRenderer.endSwapChainRenderPass(commandBuffer);
                vkeRenderer.endFrame();
            }
            vkDeviceWaitIdle(vkeDevice.device());
        }
    }

    void App::loadGameObjects()
    {

        auto skullObject = objectManager
                               .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/skull.obj")
                               .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/skull.jpg")
                               .build({-10.f, 0.5f, -10.f}, {.04f, .04f, .04f});
        gameObjects.emplace(skullObject.getId(), std::move(skullObject));

        auto eyeObject = objectManager
                             .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/eye.obj")
                             .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/eye.jpg")
                             .build({-1.f, -1.f, 0.f}, {.04f, .04f, .04f});
        gameObjects.emplace(eyeObject.getId(), std::move(eyeObject));

        auto gunObject = objectManager
                             .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/Gun.obj")
                             .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/Gun.jpg")
                             .build({2.f, 0.5f, 0.f}, {1.5f, 1.5f, 1.5f});
        gameObjects.emplace(gunObject.getId(), std::move(gunObject));

        auto quad = objectManager
                        .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/quad.obj")
                        .build({1.f, 1.f, 1.f}, {1000.f, 1000.f, 1000.f});
        gameObjects.emplace(quad.getId(), std::move(quad));

        auto sword = objectManager
                         .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/sword.obj")
                         .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/sword_albedo.jpg")
                         .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/sword_normal.jpg", TextureType::VKE_TEXTURE_TYPE_NORMAL)
                         .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/sword_roughness.jpg", TextureType::VKE_TEXTURE_TYPE_ROUGHNESS)
                         .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/sword_metallic.jpg", TextureType::VKE_TEXTURE_TYPE_METALLIC)
                         .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/sword_ao.jpg", TextureType::VKE_TEXTURE_TYPE_AO)
                         .build({0.f, -0.1f, 0.f}, {1.f, 1.f, 1.f});
        for (int i = 0; i < 10; i++)
        {
            auto swordCopy = sword;
            swordCopy.transform.translation = {4 * i, 0, 0.f};
            gameObjects.emplace(swordCopy.getId(), std::move(swordCopy));
        }
        auto phone4 = objectManager
                          .addModel(std::string(VKENGINE_ABSOLUTE_PATH) + "models/phone.obj")
                          .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/T_Telephone_Color.tga.png")
                          .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/T_Telephone_Normal.tga.png", TextureType::VKE_TEXTURE_TYPE_NORMAL)
                          .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/T_Telephone_AO.tga.png", TextureType::VKE_TEXTURE_TYPE_AO)
                          .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/T_Telephone_Metallic.tga.png", TextureType::VKE_TEXTURE_TYPE_METALLIC)
                          .addTexture(std::string(VKENGINE_ABSOLUTE_PATH) + "textures/T_Telephone_Rough.tga.png", TextureType::VKE_TEXTURE_TYPE_ROUGHNESS)
                          .build({0.f, 1.f, -6}, {10.f, 10.f, 10.f});
        gameObjects.emplace(phone4.getId(), std::move(phone4));
    }
    void App::loadLights()
    {
        std::vector<glm::vec3> lightColors{
            // {1.f, .1f, .1f},
            // {.1f, .1f, 1.f},
            // {.1f, 1.f, .1f},
            // {1.f, 1.f, .1f},
            // {.1f, 1.f, 1.f},
            // {1.f, 1.f, 1.f} //
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            auto pointLight = VkeGameObject::makePointLight(0.3f);
            pointLight.color = lightColors[i];
            auto rotate = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3{rotate * glm::vec4(-1.f, -1.f, -1.f, 1.f)};
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }
    void App::createDescriptors()
    {
        globalSetLayout = VkeDescriptorSetLayout::Builder(vkeDevice)
                              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)         // Existing UBO
                              .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // Shadow map
                              .build();
        shadowSetLayout = VkeDescriptorSetLayout::Builder(vkeDevice)
                              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT) // shadowmap UBO
                              .build();
        materialSetLayout = VkeDescriptorSetLayout::Builder(vkeDevice)
                                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // albedo
                                .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // normal
                                .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // roughness
                                .addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // metallic
                                .addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // ao
                                .build();

        globalDescriptorSets = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            VkDescriptorImageInfo shadowMapInfo{};
            shadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            shadowMapInfo.imageView = vkeRenderer.getShadowMapDepthImageView();
            auto sampler = TextureSampler(vkeDevice, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER).getSampler();
            shadowMapInfo.sampler = sampler;

            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VkeDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, &shadowMapInfo)
                .build(globalDescriptorSets[i]);
        }

        shadowDescriptorSets = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < shadowDescriptorSets.size(); i++)
        {
            auto bufferInfo = shadowUboBuffers[i]->descriptorInfo(); // shadow UBO
            VkeDescriptorWriter(*shadowSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(shadowDescriptorSets[i]);
        }

        for (auto &gameObject : gameObjects)
        {
            if (gameObject.second.material == nullptr)
            {
                continue;
            }
            auto material = gameObject.second.material;
            VkeDescriptorWriter(*materialSetLayout, *globalPool)
                .writeImage(1, &material->albedo->getDescriptor())
                .writeImage(2, &material->normal->getDescriptor())
                .writeImage(3, &material->roughness->getDescriptor())
                .writeImage(4, &material->metallic->getDescriptor())
                .writeImage(5, &material->ao->getDescriptor())
                .build(gameObject.second.descriptorSet);
        }
    }
    void App::createUBOBuffers()
    {
        uboBuffers = std::vector<std::unique_ptr<VkeBuffer>>(MAX_FRAMES_IN_FLIGHT);
        shadowUboBuffers = std::vector<std::unique_ptr<VkeBuffer>>(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<VkeBuffer>(
                vkeDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            shadowUboBuffers[i] = std::make_unique<VkeBuffer>(
                vkeDevice,
                sizeof(ShadowUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            shadowUboBuffers[i]->map();
            uboBuffers[i]->map();
        }
    }
    void App::renderImGuiFrame(VkCommandBuffer commandBuffer, VkeGameObject &sun, glm::vec3 &cameraOffset)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Sun Control");
        ImGui::SliderFloat("Sun X", &sun.transform.translation.x, -10.0f, 10.0f);
        ImGui::SliderFloat("Sun Y", &sun.transform.translation.y, -10.0f, 10.0f);
        ImGui::SliderFloat("Sun Z", &sun.transform.translation.z, -10.0f, 10.0f);

        ImGui::SliderFloat("Camera Offset X", &cameraOffset.x, -100.0f, 100.0f);
        ImGui::SliderFloat("Camera Offset Y", &cameraOffset.y, -100.0f, 100.0f);
        ImGui::SliderFloat("Camera Offset Z", &cameraOffset.z, -100.0f, 100.0f);

        ImGui::SliderFloat("Sun Color R", &sun.color.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Sun Color G", &sun.color.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Sun Color B", &sun.color.b, 0.0f, 1.0f);

        ImGui::End();

        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }
}