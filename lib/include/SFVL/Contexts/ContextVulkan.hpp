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

#include "Window.hpp"

namespace sfvl {
class ContextVulkan
{
public:
    ~ContextVulkan();

    static void createInstance(GLFWwindow* windowHandle);
    static ContextVulkan& getInstance();

private:
    static void printAvailableValidationLayers();
    static void verifyValidationLayersSupport();
    static inline std::vector<vk::LayerProperties> getAvailableValidationLayers()
    {
        return vk::enumerateInstanceLayerProperties();
    }

    static constexpr std::array VALIDATION_LAYER_NAMES{"VK_LAYER_KHRONOS_validation"};
    static constexpr std::array REQUIRED_EXTENSION_NAMES{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    /**
     * Order of preferred present modes, from most preferred to least preferred.
     *
     * TODO: customizable
     */
    static constexpr std::array PRESENT_MODES_PREFERRED{
      vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eFifoRelaxed,
      vk::PresentModeKHR::eImmediate,

      // TODO: Don't know what these do, will check later
      // vk::PresentModeKHR::eSharedDemandRefresh,
      // vk::PresentModeKHR::eSharedContinuousRefresh,
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

    using QueueFamilyPropertiesList = std::vector<vk::QueueFamilyProperties>;
    using QueueFamilyEntry = std::pair<QueueFamilyPropertiesList, QueueFamilyIndices>;

    explicit ContextVulkan(GLFWwindow* windowHandle);

    void createInstance();
    void createSurface(GLFWwindow* windowHandle);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain(GLFWwindow* windowHandle);
    void createImageViews();
    void createGraphicsPipeline();

    void chooseSwapSurfaceFormat();
    void chooseSwapPresentMode();
    void chooseSwapExtent(GLFWwindow* windowHandle);

    static bool verifyExtensionsSupport(const vk::PhysicalDevice& device);

    [[nodiscard]] QueueFamilyEntry findQueueFamilies(const vk::PhysicalDevice& physicalDevice) const noexcept;
    [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const noexcept;

    vk::Instance m_instance{};
    vk::PhysicalDevice m_physicalDevice{};
    vk::Device m_device{};
    vk::SurfaceKHR m_surface{};

    vk::SurfaceFormatKHR m_surfaceFormat{};
    vk::PresentModeKHR m_presentMode{};
    vk::Extent2D m_extent{};

    vk::Queue m_graphicsQueue{};
    vk::Queue m_presentQueue{};

    vk::SwapchainKHR m_swapChain{};
    std::vector<vk::Image> m_swapChainImages{};
    std::vector<vk::ImageView> m_swapChainImageViews{};
    vk::Format m_swapChainFormat{};

    QueueFamilyIndices m_queueFamilyIndices{};
    QueueFamilyPropertiesList m_queueFamilyProperties{};
    SwapChainSupportDetails m_swapChainSupport{};

    static std::unique_ptr<ContextVulkan> s_instance;
};
}  // namespace sfvl
