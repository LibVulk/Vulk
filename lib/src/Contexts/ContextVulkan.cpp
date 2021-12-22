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

#include "Contexts/ContextVulkan.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <string_view>

std::unique_ptr<sfvl::ContextVulkan> sfvl::ContextVulkan::s_instance{nullptr};

sfvl::ContextVulkan::ContextVulkan(GLFWwindow* windowHandle)
{
#if SFVL_DEBUG
    std::cout << "Available Layers:\n";

    for (const auto& layer : getAvailableValidationLayers())
    {
        std::cout << "\t" << layer.layerName << '\n';
    }
    std::cout.flush();

    verifyValidationLayersSupport();
#endif

    createInstance();
    createSurface(windowHandle);
    pickPhysicalDevice();
    createLogicalDevice();
}

sfvl::ContextVulkan::~ContextVulkan()
{
    if (m_surface)
        m_instance.destroy(m_surface);

    if (m_device)
        m_device.destroy();

    if (m_instance)
        m_instance.destroy();
}

void sfvl::ContextVulkan::createInstance(GLFWwindow* windowHandle)
{
    if (s_instance)
        return;

    s_instance = std::unique_ptr<ContextVulkan>(new ContextVulkan{windowHandle});
}

void sfvl::ContextVulkan::verifyValidationLayersSupport()
{
    const auto& availableLayers = getAvailableValidationLayers();

    for (const auto& layer : VALIDATION_LAYER_NAMES)
    {
        bool found = false;
        std::string_view layerName{layer};

        for (const auto& availableLayer : availableLayers)
        {
            if (layerName == availableLayer.layerName)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            // TODO: remove the validation layer instead of throwing an error
            throw std::runtime_error("Validation layer `" + std::string(layerName) + "` is not supported");
        }
    }
}

void sfvl::ContextVulkan::createInstance()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vk::ApplicationInfo appInfo{};
    vk::InstanceCreateInfo createInfo{};

    // TODO: ability to change this in the API
    appInfo.pApplicationName = "SFVL Application";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "SFVL";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

#if SFVL_DEBUG
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYER_NAMES.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYER_NAMES.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    vk::Result result = vk::createInstance(&createInfo, nullptr, &m_instance);

    if (result != vk::Result::eSuccess)
        vk::throwResultException(result, "Failed to create a Vulkan instance");
}

void sfvl::ContextVulkan::createSurface(GLFWwindow* windowHandle)
{
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(m_instance, windowHandle, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");

    m_surface = surface;
}

void sfvl::ContextVulkan::pickPhysicalDevice()
{
    const auto& devices = m_instance.enumeratePhysicalDevices();

    if (devices.empty())
        throw std::runtime_error("No Vulkan-compatible GPUs found");

    for (const auto& device : devices)
    {
        auto [list, indices] = findQueueFamilies(device);

        if (indices.isValid())
        {
            m_physicalDevice = device;
            m_queueFamilyProperties = std::move(list);
            m_queueFamilyIndices = indices;
            break;
        }
    }

    if (!m_physicalDevice)
        throw std::runtime_error("Failed to find an appropriate GPU");
}

void sfvl::ContextVulkan::createLogicalDevice()
{
    static constexpr float queuePriority = 1.f;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfoList{};
    vk::DeviceCreateInfo createInfo{};
    vk::PhysicalDeviceFeatures features{};

    const std::set<uint32_t> uniqueQueueFamilies = {m_queueFamilyIndices.graphicsFamily.value(),
                                                    m_queueFamilyIndices.presentFamily.value()};

    for (const auto& family : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo{};

        queueCreateInfo.queueCount = 1;  // TODO: support more
        queueCreateInfo.queueFamilyIndex = family;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfoList.push_back(queueCreateInfo);
    }

    createInfo.pEnabledFeatures = &features;
    createInfo.pQueueCreateInfos = queueCreateInfoList.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
    createInfo.enabledLayerCount = 0;
    createInfo.enabledExtensionCount = 0;

    m_device = m_physicalDevice.createDevice(createInfo);

    if (!m_device)
        throw std::runtime_error("Failed to create logical device");

    m_device.getQueue(m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);

    if (!m_graphicsQueue)
        throw std::runtime_error("Failed to retrieve graphics queue");

    m_device.getQueue(m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);

    if (!m_presentQueue)
        throw std::runtime_error("Failed to retrieve present queue");
}

sfvl::ContextVulkan::QueueFamilyEntry sfvl::ContextVulkan::findQueueFamilies(const vk::PhysicalDevice& physicalDevice)
{
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    QueueFamilyIndices indices{};

    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 supported;
        if (physicalDevice.getSurfaceSupportKHR(i, m_surface, &supported) == vk::Result::eSuccess && supported)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;
    }

    return std::make_pair(std::move(queueFamilies), indices);
}

sfvl::ContextVulkan& sfvl::ContextVulkan::getInstance()
{
    return *s_instance;
}
