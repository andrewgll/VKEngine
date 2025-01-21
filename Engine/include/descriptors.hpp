#pragma once

#include "device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

namespace vke
{

    class VkeDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(VkeDevice &vkeDevice) : vkeDevice{vkeDevice} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<VkeDescriptorSetLayout> build() const;

        private:
            VkeDevice &vkeDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        VkeDescriptorSetLayout(
            VkeDevice &vkeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~VkeDescriptorSetLayout();
        VkeDescriptorSetLayout(const VkeDescriptorSetLayout &) = delete;
        VkeDescriptorSetLayout &operator=(const VkeDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        VkeDevice &vkeDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class VkeDescriptorWriter;
    };

    class VkeDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(VkeDevice &vkeDevice) : vkeDevice{vkeDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<VkeDescriptorPool> build() const;

        private:
            VkeDevice &vkeDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        VkeDescriptorPool(
            VkeDevice &vkeDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~VkeDescriptorPool();
        VkeDescriptorPool(const VkeDescriptorPool &) = delete;
        VkeDescriptorPool &operator=(const VkeDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        VkeDevice &vkeDevice;
        VkDescriptorPool descriptorPool;

        friend class VkeDescriptorWriter;
    };

    class VkeDescriptorWriter
    {
    public:
        VkeDescriptorWriter(VkeDescriptorSetLayout &setLayout, VkeDescriptorPool &pool);

        VkeDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        VkeDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        VkeDescriptorSetLayout &setLayout;
        VkeDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace vke