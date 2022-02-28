/*
 * Copyright (c) 2021-2021 [fill name later]
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 *     will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 *     applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented
 * as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include <vulkan/vulkan.hpp>

#include <array>
#include <memory>
#include <optional>

#include "Vulk/Exceptions.hpp"
#include "Vulk/Graphics/ADrawable.hpp"
#include "Vulk/Graphics/AShape.hpp"
#include "Vulk/Macros.hpp"
#include "Vulk/Objects.hpp"
#include "Vulk/Window.hpp"

namespace vulk::detail {
class ContextVulkan
{
public:
    ~ContextVulkan();

    void draw();

    template<typename T>
    void createBuffers(const std::vector<T>& vertices, vk::Buffer& outBuffer, vk::DeviceMemory& outDeviceMemory,
                       vk::BufferUsageFlagBits flags)
    {
        const size_t bufferSize = sizeof(std::remove_cvref_t<decltype(vertices)>::value_type) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer, stagingBufferMemory);

        void* data;
        handleVulkanError(m_device.mapMemory(stagingBufferMemory, 0, bufferSize, {}, &data));
        std::memcpy(data, vertices.data(), bufferSize);
        m_device.unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | flags,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, outBuffer,
                     outDeviceMemory);

        copyBuffer(stagingBuffer, outBuffer, bufferSize);

        // TODO: vk::raii
        m_device.destroy(stagingBuffer);
        m_device.freeMemory(stagingBufferMemory);
    }

    void destroyBuffer(vk::Buffer buffer);
    void freeBufferMem(vk::DeviceMemory deviceMemory);

    void registerDrawable(AShape& drawable);
    void unregisterDrawable(AShape& drawable);

    // TODO: should not be public, remove once events are implemented
    void setFrameBufferResized(bool value) noexcept { m_frameBufferResized = value; }

    static void createInstance(GLFWwindow* windowHandle);
    static ContextVulkan& getInstance();

    VULK_NO_MOVE_OR_COPY(ContextVulkan)

private:
    static void printAvailableValidationLayers();
    static std::vector<const char*> getSupportedValidationLayers();

    static constexpr std::array VALIDATION_LAYER_NAMES{"VK_LAYER_KHRONOS_validation"};
    static constexpr std::array REQUIRED_EXTENSION_NAMES{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    /**
     * Order of preferred present modes, from most preferred to least preferred.
     *
     * TODO: customizable
     */
    static constexpr std::array PRESENT_MODES_PREFERRED{
      vk::PresentModeKHR::eMailbox,      //
      vk::PresentModeKHR::eFifo,         //
      vk::PresentModeKHR::eFifoRelaxed,  //
      vk::PresentModeKHR::eImmediate,    //

      // vk::PresentModeKHR::eSharedDemandRefresh, //
      // vk::PresentModeKHR::eSharedContinuousRefresh, //
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        /**
         * @return true if the structure is complete
         */
        [[nodiscard]] bool isComplete() const noexcept
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }

        // return isComplete() for now, will probably change later
        /**
         * @return true if the structure is valid
         */
        [[nodiscard]] bool isValid() const noexcept { return isComplete(); }
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities{};
        std::vector<vk::SurfaceFormatKHR> formats{};
        std::vector<vk::PresentModeKHR> presentModes{};

        [[nodiscard]] bool isValid() const noexcept { return !formats.empty() && !presentModes.empty(); }
    };

    struct FrameSyncObjects
    {
        vk::Semaphore imageAvailable{};
        vk::Semaphore renderFinished{};
        vk::Fence fence{};

        void destroy(vk::Device& device)
        {
            device.destroy(imageAvailable);
            device.destroy(renderFinished);
            device.destroy(fence);
        }
    };

    using QueueFamilyPropertiesList = std::vector<vk::QueueFamilyProperties>;
    using QueueFamilyEntry = std::pair<QueueFamilyPropertiesList, QueueFamilyIndices>;

    explicit ContextVulkan(GLFWwindow* windowHandle);

    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFrameBuffers();
    void createCommandPool();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObject();

    void recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);

    void recreateSwapChain();
    void copyBuffer(const vk::Buffer& sourceBuffer, vk::Buffer& destinationBuffer, vk::DeviceSize size);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                      vk::Buffer& outBuffer, vk::DeviceMemory& outDeviceMemory);

    void cleanupSwapchain(vk::SwapchainKHR& swapchain);
    void cleanupSwapchainSubObjects();

    void chooseSwapSurfaceFormat();
    void chooseSwapPresentMode();
    void chooseSwapExtent();

    void updateUniformBuffer(uint32_t currentImage);

    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

    static bool verifyExtensionsSupport(const vk::PhysicalDevice& device);

    [[nodiscard]] QueueFamilyEntry findQueueFamilies(const vk::PhysicalDevice& physicalDevice) const noexcept;
    [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const noexcept;

    // TODO: cache image count used in framebuffers, swap-chain, etc

    // TODO: the ContextVulkan should not contain the raw window handle, maybe a Window reference though
    GLFWwindow* m_windowHandle;

    vk::Instance m_instance{};
    vk::PhysicalDevice m_physicalDevice{};
    vk::Device m_device{};
    vk::SurfaceKHR m_surface{};

    vk::SurfaceFormatKHR m_surfaceFormat{};
    vk::PresentModeKHR m_presentMode{};
    vk::Extent2D m_extent{};

    vk::Queue m_graphicsQueue{};
    vk::Queue m_presentQueue{};

    vk::SwapchainKHR m_swapchain{};
    std::vector<vk::Image> m_swapchainImages{};
    std::vector<vk::ImageView> m_swapchainImageViews{};
    vk::Format m_swapchainFormat{};

    vk::Viewport m_viewport{};
    vk::RenderPass m_renderPass{};

    vk::DescriptorSetLayout m_descriptorSetLayout{};
    vk::PipelineLayout m_pipelineLayout{};
    vk::Pipeline m_pipeline{};

    QueueFamilyIndices m_queueFamilyIndices{};
    QueueFamilyPropertiesList m_queueFamilyProperties{};
    SwapChainSupportDetails m_swapchainSupport{};

    std::vector<vk::Framebuffer> m_swapchainFrameBuffers{};

    vk::CommandPool m_commandPool{};
    std::vector<vk::CommandBuffer> m_commandBuffers{};

    std::vector<FrameSyncObjects> m_frameSyncObjects{};
    std::vector<vk::Fence> m_imagesInFlight{};

    std::vector<vk::Buffer> m_uniformBuffers{};
    std::vector<vk::DeviceMemory> m_uniformBuffersMemory{};

    vk::DescriptorPool m_descriptorPool{};
    std::vector<vk::DescriptorSet> m_descriptorSets{};

    std::vector<vulk::AShape*> m_shapes{};

    // TODO: May be better to store in the FrameManager
    size_t m_currentFrame{};
    static const size_t s_maxFramesInFlight;
    //

    bool m_frameBufferResized{false};

    static constexpr auto NO_TIMEOUT = std::numeric_limits<uint64_t>::max();

    static std::unique_ptr<ContextVulkan> s_instance;
};
}  // namespace vulk::detail
