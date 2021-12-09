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

    static void verifyValidationLayersSupport();
    static inline std::vector<vk::LayerProperties> getAvailableValidationLayers()
    {
        return vk::enumerateInstanceLayerProperties();
    }

    static constexpr std::array VALIDATION_LAYER_NAMES{"VK_LAYER_KHRONOS_validation"};

private:
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

    using QueueFamilyPropertiesList = std::vector<vk::QueueFamilyProperties>;
    using QueueFamilyEntry = std::pair<QueueFamilyPropertiesList, QueueFamilyIndices>;

    explicit ContextVulkan(GLFWwindow* windowHandle);

    void createInstance();
    void createSurface(GLFWwindow* windowHandle);
    void pickPhysicalDevice();
    void createLogicalDevice();

    [[nodiscard]] QueueFamilyEntry findQueueFamilies(const vk::PhysicalDevice& physicalDevice);

    vk::Instance m_instance{VK_NULL_HANDLE};
    vk::PhysicalDevice m_physicalDevice{VK_NULL_HANDLE};
    vk::Device m_device{VK_NULL_HANDLE};
    vk::SurfaceKHR m_surface{VK_NULL_HANDLE};

    vk::Queue m_graphicsQueue{VK_NULL_HANDLE};
    vk::Queue m_presentQueue{VK_NULL_HANDLE};

    QueueFamilyPropertiesList m_queueFamilyProperties{};
    QueueFamilyIndices m_queueFamilyIndices{};

    static std::unique_ptr<ContextVulkan> s_instance;
};
}  // namespace sfvl
