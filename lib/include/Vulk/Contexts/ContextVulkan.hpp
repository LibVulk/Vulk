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

#include "Vulk/ClassUtils.hpp"
#include "Vulk/Vertex.hpp"
#include "Vulk/Window.hpp"

namespace vulk {
class ContextVulkan
{
public:
    ~ContextVulkan();

    /**
     * TODO: This should not be public. The Vulkan context should only be exposed to the window.
     * Calling this function from the static `getInstance().draw()` can break stuff.
     */
    void draw();

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
    void createGraphicsPipeline();
    void createFrameBuffers();
    void createCommandPool();
    void createVertexBuffer();
    void createIndexBuffer();
    void createCommandBuffers();
    void createSyncObject();

    void recreateSwapChain();
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
                      vk::Buffer& outBuffer, vk::DeviceMemory& outDeviceMemory);
    void copyBuffer(const vk::Buffer& sourceBuffer, vk::Buffer& destinationBuffer, vk::DeviceSize size);

    void cleanupSwapchain(vk::SwapchainKHR& swapchain);
    void cleanupSwapchainSubObjects();

    void chooseSwapSurfaceFormat();
    void chooseSwapPresentMode();
    void chooseSwapExtent();

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

    vk::Buffer m_vertexBuffer{};
    vk::DeviceMemory m_vertexBufferMemory{};
    vk::Buffer m_indexBuffer{};
    vk::DeviceMemory m_indexBufferMemory{};

    // TODO: May be better to store in the FrameManager
    size_t m_currentFrame{};
    static const size_t s_maxFramesInFlight;
    //

    bool m_frameBufferResized{false};

    template<typename T>
    static constexpr vk::IndexType getIndexType()
    {
        // could be a concept with C++20
        static_assert(std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>);

        if (std::is_same_v<T, uint16_t>)
            return vk::IndexType::eUint16;
        if (std::is_same_v<T, uint32_t>)
            return vk::IndexType::eUint32;

        assert(0);
        return vk::IndexType::eNoneKHR;
    }

    static constexpr std::array s_vertices{Vertex{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  //
                                           Vertex{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   //
                                           Vertex{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    //
                                           Vertex{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
    static constexpr std::array<uint16_t, 6> s_indices{0, 1, 2, 2, 3, 0};

    static std::unique_ptr<ContextVulkan> s_instance;
};
}  // namespace vulk
