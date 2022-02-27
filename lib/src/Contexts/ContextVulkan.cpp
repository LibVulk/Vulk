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

#include "Vulk/Contexts/ContextVulkan.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <iostream>
#include <set>
#include <string_view>

#include "Vulk/Exceptions.hpp"
#include "Vulk/ScopedProfiler.hpp"
#include "Vulk/Shader.hpp"
#include "Vulk/Utils.hpp"

const size_t vulk::ContextVulkan::s_maxFramesInFlight{2};
std::unique_ptr<vulk::ContextVulkan> vulk::ContextVulkan::s_instance{nullptr};

// TODO: This should be in the Window class and then fire an event, once those are implemented.
static void framebufferResizeCallback(GLFWwindow* windowHandle, int, int)
{
    auto* context = reinterpret_cast<vulk::ContextVulkan*>(glfwGetWindowUserPointer(windowHandle));
    assert(context);

    context->setFrameBufferResized(true);
}

vulk::ContextVulkan::ContextVulkan(GLFWwindow* windowHandle) : m_windowHandle{windowHandle}
{
    VULK_SCOPED_PROFILER("ContextVulkan::ContextVulkan()");

    assert(m_windowHandle);

    glfwSetWindowUserPointer(m_windowHandle, this);  // ugly workaround until events are implemented
    glfwSetFramebufferSizeCallback(m_windowHandle, &framebufferResizeCallback);

#if VULK_DEBUG
    // printAvailableValidationLayers();
#endif

    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObject();

#if VULK_DEBUG
    std::cout << "Selected GPU name: " << m_physicalDevice.getProperties().deviceName << std::endl;
#endif
}

vulk::ContextVulkan::~ContextVulkan()
{
    VULK_SCOPED_PROFILER("ContextVulkan::~ContextVulkan()");

    // TODO: use vk::raii

    if (m_device)
    {
        m_device.waitIdle();

        cleanupSwapchain(m_swapchain);

        for (size_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            m_device.destroy(m_uniformBuffers[i]);
            m_device.freeMemory(m_uniformBuffersMemory[i]);
        }

        m_device.destroy(m_descriptorPool);
        m_device.destroy(m_descriptorSetLayout);

        for (auto& frameSemaphore : m_frameSyncObjects)
            frameSemaphore.destroy(m_device);

        m_device.destroy(m_commandPool);

        m_device.destroy(m_indexBuffer);
        m_device.freeMemory(m_indexBufferMemory);

        m_device.destroy(m_vertexBuffer);
        m_device.freeMemory(m_vertexBufferMemory);
    }

    m_instance.destroy(m_surface);
    m_device.destroy();
    m_instance.destroy();
}

void vulk::ContextVulkan::cleanupSwapchain(vk::SwapchainKHR& swapchain)
{
    if (!m_device)
        return;

    for (auto& framebuffer : m_swapchainFrameBuffers)
        m_device.destroy(framebuffer);
    m_swapchainFrameBuffers.clear();

    m_device.freeCommandBuffers(m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

    // If the cleaned-up swapchain differs from the current one,
    // don't release resources in use: we are reusing the old stuff
    if (swapchain == m_swapchain)
    {
        cleanupSwapchainSubObjects();
    }

    m_device.destroy(swapchain);
    swapchain = nullptr;
}

void vulk::ContextVulkan::cleanupSwapchainSubObjects()
{
    if (!m_device)
        return;

    m_device.destroy(m_pipeline);
    m_device.destroy(m_pipelineLayout);
    m_device.destroy(m_renderPass);

    for (auto& imageView : m_swapchainImageViews)
        m_device.destroy(imageView);
    m_swapchainImageViews.clear();
}

void vulk::ContextVulkan::createInstance(GLFWwindow* windowHandle)
{
    assert(windowHandle);

    if (s_instance)
        return;

    s_instance = std::unique_ptr<ContextVulkan>(new ContextVulkan{windowHandle});
    assert(s_instance);
}

void vulk::ContextVulkan::draw()
{
    handleVulkanError(m_device.waitForFences(1, &m_frameSyncObjects[m_currentFrame].fence, true, s_noTimeout));

    const auto& [result, imageIndex] =
      m_device.acquireNextImageKHR(m_swapchain, s_noTimeout, m_frameSyncObjects[m_currentFrame].imageAvailable);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
        handleVulkanError(result);  // throws
    }

    updateUniformBuffer(static_cast<uint32_t>(m_currentFrame));
    handleVulkanError(m_device.resetFences(1, &m_frameSyncObjects[m_currentFrame].fence));
    m_commandBuffers[m_currentFrame].reset();
    recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

    const std::array swapChains{m_swapchain};
    const std::array waitSemaphores{m_frameSyncObjects[m_currentFrame].imageAvailable};
    const std::array signalSemaphores{m_frameSyncObjects[m_currentFrame].renderFinished};
    const vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    handleVulkanError(m_graphicsQueue.submit(1, &submitInfo, m_frameSyncObjects[m_currentFrame].fence));

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    presentInfo.pWaitSemaphores = signalSemaphores.data();
    presentInfo.swapchainCount = static_cast<uint32_t>(swapChains.size());
    presentInfo.pSwapchains = swapChains.data();
    presentInfo.pImageIndices = &imageIndex;
    // presentInfo.pResults = nullptr; // would be useful if ever adding more swap chains

    const vk::Result res = m_presentQueue.presentKHR(&presentInfo);

    if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR || m_frameBufferResized)
    {
        m_frameBufferResized = false;
        recreateSwapChain();
    } else
    {
        handleVulkanError(res);  // Throws if res != eSuccess
    }

    m_currentFrame = (m_currentFrame + 1) % s_maxFramesInFlight;
}

[[maybe_unused]] void vulk::ContextVulkan::printAvailableValidationLayers()
{
    std::cout << "Available Layers:\n";

    for (const auto& layer : vk::enumerateInstanceLayerProperties())
    {
        std::cout << '\t' << layer.layerName << '\n';
    }
    std::cout.flush();
}

std::vector<const char*> vulk::ContextVulkan::getSupportedValidationLayers()
{
    const auto& availableLayers = vk::enumerateInstanceLayerProperties();
    std::vector<const char*> supported{};

    for (const auto& layer : VALIDATION_LAYER_NAMES)
    {
        bool found = false;
        std::string_view layerName{layer};

        for (const auto& availableLayer : availableLayers)
        {
            if (layerName == availableLayer.layerName)
            {
                supported.push_back(layer);
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Warning: Validation layer `" << layerName << "` is not supported.\n";
        }
    }

    return supported;
}

void vulk::ContextVulkan::createInstance()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createInstance()");

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vk::ApplicationInfo appInfo{};
    vk::InstanceCreateInfo createInfo{};

    // TODO: ability to change this in the API
    appInfo.pApplicationName = "Vulk Application";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Vulk";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

#if VULK_DEBUG
    const auto& validationLayers = getSupportedValidationLayers();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    handleVulkanError(vk::createInstance(&createInfo, nullptr, &m_instance));
}

void vulk::ContextVulkan::createSurface()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createSurface()");

    VkSurfaceKHR surface;

    handleVulkanError(glfwCreateWindowSurface(m_instance, m_windowHandle, nullptr, &surface));

    m_surface = surface;
}

void vulk::ContextVulkan::pickPhysicalDevice()
{
    VULK_SCOPED_PROFILER("ContextVulkan::pickPhysicalDevice()");

    const auto& devices = m_instance.enumeratePhysicalDevices();

    if (devices.empty())
        throw VulkanException("No Vulkan-compatible GPUs found");

    for (const auto& device : devices)
    {
        const auto& [list, indices] = findQueueFamilies(device);
        const auto& swapChainSupportDetails = querySwapChainSupport(device);

        // For now, we pick the first device we get.
        // Ideally, we should pick a dedicated GPU in case the CPU also does GPU.
        // Long term, we could even let the developers / users pick for themselves.
        if (indices.isValid() && verifyExtensionsSupport(device) && swapChainSupportDetails.isValid())
        {
            m_physicalDevice = device;
            m_queueFamilyIndices = indices;
            m_queueFamilyProperties = list;
            break;
        }
    }

    if (!m_physicalDevice)
        throw VulkanException("Failed to find an appropriate GPU");
}

void vulk::ContextVulkan::createLogicalDevice()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createLogicalDevice()");

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

    // This could be better: here we only use static extensions at compile time, would be better to fetch them dynamically.
    createInfo.enabledExtensionCount = static_cast<uint32_t>(REQUIRED_EXTENSION_NAMES.size());
    createInfo.ppEnabledExtensionNames = REQUIRED_EXTENSION_NAMES.data();

    handleVulkanError(m_physicalDevice.createDevice(&createInfo, nullptr, &m_device));

    m_device.getQueue(m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);

    if (!m_graphicsQueue)
        throw VulkanException("Failed to retrieve graphics queue");

    m_device.getQueue(m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);

    if (!m_presentQueue)
        throw VulkanException("Failed to retrieve present queue");
}

void vulk::ContextVulkan::recreateSwapChain()
{
    {
        int width, height;

        glfwGetWindowSize(m_windowHandle, &width, &height);

        // App is minimized or has an invalid size, ignore and wait for the next resize
        // TODO: This will pause execution, needs to be customizable, and probably off by default
        while (width == 0 || height == 0)
        {
            glfwWaitEvents();
            glfwGetWindowSize(m_windowHandle, &width, &height);
        }
    }

    m_device.waitIdle();

    cleanupSwapchainSubObjects();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
}

void vulk::ContextVulkan::createSwapChain()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createSwapChain()");

    m_swapchainSupport = querySwapChainSupport(m_physicalDevice);

    chooseSwapSurfaceFormat();
    chooseSwapPresentMode();
    chooseSwapExtent();

    uint32_t imageCount = m_swapchainSupport.capabilities.minImageCount + 1;
    if (m_swapchainSupport.capabilities.maxImageCount > 0 && imageCount > m_swapchainSupport.capabilities.maxImageCount)
    {
        imageCount = m_swapchainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainKHR oldSwapchain = m_swapchain;  // useful when recreating the swapchain
    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_surfaceFormat.format;
    createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;  // vk::ImageUsageFlagBits::eTransferDst later?
    createInfo.preTransform = m_swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;  // customizable later?
    createInfo.presentMode = m_presentMode;
    createInfo.clipped = true;

    // Reuse old swap chain if it exists.
    // Fixes a vk::Result::eDeviceLost that would (sometimes) throw when presenting.
    createInfo.oldSwapchain = m_swapchain;

    const uint32_t queueFamilyIndices[] = {m_queueFamilyIndices.graphicsFamily.value(),
                                           m_queueFamilyIndices.presentFamily.value()};

    if (m_queueFamilyIndices.graphicsFamily != m_queueFamilyIndices.presentFamily)
    {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    handleVulkanError(m_device.createSwapchainKHR(&createInfo, nullptr, &m_swapchain));

    if (oldSwapchain)
        cleanupSwapchain(oldSwapchain);

    m_swapchainImages = m_device.getSwapchainImagesKHR(m_swapchain);
    m_swapchainFormat = m_surfaceFormat.format;
}

void vulk::ContextVulkan::createImageViews()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createImageViews()");

    const auto size = m_swapchainImages.size();
    m_swapchainImageViews.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        vk::ImageViewCreateInfo createInfo{};

        createInfo.image = m_swapchainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_swapchainFormat;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        m_swapchainImageViews.push_back(m_device.createImageView(createInfo, nullptr));
    }
}

void vulk::ContextVulkan::createRenderPass()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createRenderPass()");

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapchainFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;  // TODO: modifiable
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = {};
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    handleVulkanError(m_device.createRenderPass(&renderPassCreateInfo, nullptr, &m_renderPass));
    assert(m_renderPass);
}

void vulk::ContextVulkan::createDescriptorSetLayout()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createDescriptorSetLayout()");

    vk::DescriptorSetLayoutBinding uboLayoutBindings{};
    uboLayoutBindings.binding = 0;
    uboLayoutBindings.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBindings.descriptorCount = 1;
    uboLayoutBindings.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutCreateInfo createInfo{};
    createInfo.bindingCount = 1;
    createInfo.pBindings = &uboLayoutBindings;

    handleVulkanError(m_device.createDescriptorSetLayout(&createInfo, nullptr, &m_descriptorSetLayout));
}

void vulk::ContextVulkan::createGraphicsPipeline()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createGraphicsPipeline()");

    Shader vert{m_device, "shaders/vulk/shader.vert.spv", Shader::Type::eVertex};
    Shader frag{m_device, "shaders/vulk/shader.frag.spv", Shader::Type::eFragment};

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vert.getShaderStageCreateInfo(),
                                                        frag.getShaderStageCreateInfo()};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;

    m_viewport.x = 0;
    m_viewport.y = 0;
    m_viewport.width = static_cast<float>(m_extent.width);
    m_viewport.height = static_cast<float>(m_extent.height);
    m_viewport.minDepth = 0;
    m_viewport.maxDepth = 1;

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = m_extent;

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &m_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = false;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::array dynamicStatesArray = {vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStatesArray.size());
    dynamicState.pDynamicStates = dynamicStatesArray.data();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;

    handleVulkanError(m_device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;

    // Useless now, but will be useful and more efficient when modifying the pipeline
    // I added it as a reminder :)
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    handleVulkanError(m_device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &m_pipeline));
}

void vulk::ContextVulkan::createFrameBuffers()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createFrameBuffers()");

    m_swapchainFrameBuffers.reserve(m_swapchainImageViews.size());

    for (const auto& imageView : m_swapchainImageViews)
    {
        vk::ImageView attachments[] = {imageView};

        vk::FramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.renderPass = m_renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = m_extent.width;
        framebufferCreateInfo.height = m_extent.height;
        framebufferCreateInfo.layers = 1;

        vk::Framebuffer framebuffer;
        handleVulkanError(m_device.createFramebuffer(&framebufferCreateInfo, nullptr, &framebuffer));
        assert(framebuffer);

        m_swapchainFrameBuffers.push_back(framebuffer);
    }
}

void vulk::ContextVulkan::createCommandPool()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createCommandPool()");

    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    handleVulkanError(m_device.createCommandPool(&commandPoolCreateInfo, nullptr, &m_commandPool));
}

void vulk::ContextVulkan::createVertexBuffer()
{
    static constexpr vk::DeviceSize BufferSize = sizeof(decltype(s_vertices)::value_type) * s_vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    createBuffer(BufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer,
                 stagingBufferMemory);

    void* data;
    handleVulkanError(m_device.mapMemory(stagingBufferMemory, 0, BufferSize, {}, &data));
    std::memcpy(data, s_vertices.data(), BufferSize);
    m_device.unmapMemory(stagingBufferMemory);

    createBuffer(BufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, m_vertexBuffer,
                 m_vertexBufferMemory);

    copyBuffer(stagingBuffer, m_vertexBuffer, BufferSize);

    // TODO: vk::raii
    m_device.destroy(stagingBuffer);
    m_device.freeMemory(stagingBufferMemory);
}

void vulk::ContextVulkan::createIndexBuffer()
{
    static constexpr vk::DeviceSize BufferSize = sizeof(decltype(s_indices)::value_type) * s_indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    createBuffer(BufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer,
                 stagingBufferMemory);

    void* data;
    handleVulkanError(m_device.mapMemory(stagingBufferMemory, 0, BufferSize, {}, &data));
    std::memcpy(data, s_indices.data(), BufferSize);
    m_device.unmapMemory(stagingBufferMemory);

    createBuffer(BufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, m_indexBuffer,
                 m_indexBufferMemory);

    copyBuffer(stagingBuffer, m_indexBuffer, BufferSize);

    // TODO: vk::raii
    m_device.destroy(stagingBuffer);
    m_device.freeMemory(stagingBufferMemory);
}

void vulk::ContextVulkan::createUniformBuffers()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createUniformBuffers()");

    static constexpr size_t BUFFER_SIZE = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(s_maxFramesInFlight);
    m_uniformBuffersMemory.resize(s_maxFramesInFlight);

    for (size_t i = 0; i < s_maxFramesInFlight; ++i)
    {
        createBuffer(BUFFER_SIZE, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     m_uniformBuffers[i], m_uniformBuffersMemory[i]);
    }
}

void vulk::ContextVulkan::createDescriptorPool()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createDescriptorPool()");

    vk::DescriptorPoolSize poolSize{};
    poolSize.descriptorCount = static_cast<uint32_t>(s_maxFramesInFlight);

    vk::DescriptorPoolCreateInfo createInfo{};
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = static_cast<uint32_t>(s_maxFramesInFlight);

    handleVulkanError(m_device.createDescriptorPool(&createInfo, nullptr, &m_descriptorPool));
}

void vulk::ContextVulkan::createDescriptorSets()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createDescriptorSets()");

    std::vector<vk::DescriptorSetLayout> layouts(s_maxFramesInFlight, m_descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.descriptorPool = m_descriptorPool;
    allocateInfo.descriptorSetCount = static_cast<uint32_t>(s_maxFramesInFlight);
    allocateInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(s_maxFramesInFlight);
    handleVulkanError(m_device.allocateDescriptorSets(&allocateInfo, m_descriptorSets.data()));

    for (size_t i = 0; i < s_maxFramesInFlight; ++i)
    {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = m_descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        m_device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}

void vulk::ContextVulkan::createCommandBuffers()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createCommandBuffers()");

    m_commandBuffers.resize(m_swapchainFrameBuffers.size());

    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(m_swapchainFrameBuffers.size());

    handleVulkanError(m_device.allocateCommandBuffers(&allocateInfo, m_commandBuffers.data()));
}

void vulk::ContextVulkan::createSyncObject()
{
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};

    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    m_imagesInFlight.resize(m_swapchainImages.size());
    m_frameSyncObjects.resize(s_maxFramesInFlight);

    for (auto& frameSyncObj : m_frameSyncObjects)
    {
        handleVulkanError(m_device.createSemaphore(&semaphoreInfo, nullptr, &frameSyncObj.imageAvailable));
        handleVulkanError(m_device.createSemaphore(&semaphoreInfo, nullptr, &frameSyncObj.renderFinished));
        handleVulkanError(m_device.createFence(&fenceInfo, nullptr, &frameSyncObj.fence));
    }
}

void vulk::ContextVulkan::recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
{
    vk::CommandBufferBeginInfo beginInfo{};
    // beginInfo.flags = vk::CommandBufferUsageFlagBits::...;
    // beginInfo.pInheritanceInfo = nullptr;

    handleVulkanError(commandBuffer.begin(&beginInfo));

    vk::ClearValue clearValue{};
    clearValue.color = vk::ClearColorValue{std::array{0.f, 0.f, 0.f, 1.f}};

    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.renderPass = m_renderPass;
    renderPassBeginInfo.framebuffer = m_swapchainFrameBuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassBeginInfo.renderArea.extent = m_extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    std::array vertexBuffers{m_vertexBuffer};
    std::array offsets{vk::DeviceSize{0}};
    static_assert(vertexBuffers.size() == offsets.size());

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1,
                                     &m_descriptorSets[m_currentFrame], 0, nullptr);
    commandBuffer.bindVertexBuffers(0, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(),
                                    offsets.data());
    commandBuffer.bindIndexBuffer(m_indexBuffer, 0, getIndexType<decltype(s_indices)::value_type>());
    commandBuffer.drawIndexed(static_cast<uint32_t>(s_indices.size()), 1, 0, 0, 0);
    commandBuffer.endRenderPass();
    commandBuffer.end();
}

void vulk::ContextVulkan::chooseSwapSurfaceFormat()
{
    assert(!m_swapchainSupport.formats.empty());

    for (const auto& format : m_swapchainSupport.formats)
    {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            m_surfaceFormat = format;
            return;
        }
    }

#if VULK_DEBUG
    std::cerr << "Warning: unexpected format support, using the first available one.\n";
#endif
    m_surfaceFormat = m_swapchainSupport.formats[0];
}

void vulk::ContextVulkan::chooseSwapPresentMode()
{
    for (const auto& presentMode : PRESENT_MODES_PREFERRED)
    {
        // This can be expensive to search the list again and again,
        // but since the list is less than 5 elements and is done once, shouldn't matter too much.

        if (utils::vectorContains(m_swapchainSupport.presentModes, presentMode))
        {
            m_presentMode = presentMode;
            return;
        }
    }

    // Should not happen
    throw VulkanException("No present mode available.");
}

void vulk::ContextVulkan::chooseSwapExtent()
{
    const auto& caps = m_swapchainSupport.capabilities;

    if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_extent = caps.currentExtent;
    } else
    {
        int width, height;

        glfwGetFramebufferSize(m_windowHandle, &width, &height);

        m_extent = vk::Extent2D{
          std::clamp(static_cast<uint32_t>(width), caps.minImageExtent.width, caps.maxImageExtent.width),
          std::clamp(static_cast<uint32_t>(height), caps.minImageExtent.height, caps.maxImageExtent.height)};
    }
}

void vulk::ContextVulkan::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = Clock::now();
    auto currentTime = Clock::now();
    auto delta = vulk::Duration{currentTime - startTime}.count();

    UniformBufferObject ubo{
      glm::rotate(glm::mat4(1.0f), delta * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::perspective(glm::radians(45.0f), static_cast<float>(m_extent.width) / static_cast<float>(m_extent.height),
                       0.1f, 10.0f)};
    ubo.projection[1][1] *= -1;

    void* data;
    handleVulkanError(m_device.mapMemory(m_uniformBuffersMemory[currentImage], 0, sizeof(ubo), {}, &data));
    std::memcpy(data, &ubo, sizeof(ubo));
    m_device.unmapMemory(m_uniformBuffersMemory[currentImage]);
}

void vulk::ContextVulkan::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                       vk::MemoryPropertyFlags properties, vk::Buffer& outBuffer,
                                       vk::DeviceMemory& outDeviceMemory)
{
    VULK_SCOPED_PROFILER("ContextVulkan::createBuffer()");

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    handleVulkanError(m_device.createBuffer(&bufferInfo, nullptr, &outBuffer));

    vk::MemoryRequirements memoryRequirements{m_device.getBufferMemoryRequirements(outBuffer)};
    vk::MemoryAllocateInfo allocateInfo{};
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    handleVulkanError(m_device.allocateMemory(&allocateInfo, nullptr, &outDeviceMemory));
    m_device.bindBufferMemory(outBuffer, outDeviceMemory, 0);
}

void vulk::ContextVulkan::copyBuffer(const vk::Buffer& sourceBuffer, vk::Buffer& destinationBuffer, vk::DeviceSize size)
{
    VULK_SCOPED_PROFILER("ContextVulkan::copyBuffer()");

    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    handleVulkanError(m_device.allocateCommandBuffers(&allocateInfo, &commandBuffer));

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    handleVulkanError(commandBuffer.begin(&beginInfo));

    vk::BufferCopy copyRegion{};
    copyRegion.size = size;

    commandBuffer.copyBuffer(sourceBuffer, destinationBuffer, 1, &copyRegion);
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    handleVulkanError(m_graphicsQueue.submit(1, &submitInfo, nullptr));
    m_graphicsQueue.waitIdle();

    // TODO: vk::raii
    m_device.freeCommandBuffers(m_commandPool, 1, &commandBuffer);
}

bool vulk::ContextVulkan::verifyExtensionsSupport(const vk::PhysicalDevice& device)
{
    VULK_SCOPED_PROFILER("ContextVulkan::verifyExtensionsSupport()");

    const auto& currentExts = device.enumerateDeviceExtensionProperties();

    bool allValid = true;
    for (const auto& extension : REQUIRED_EXTENSION_NAMES)
    {
        const bool valid = std::find_if(currentExts.cbegin(), currentExts.cend(), [&extension](const auto& props) {
                               return std::string_view{props.extensionName} == std::string_view{extension};
                           }) != currentExts.cend();

        if (!valid)
            std::cerr << "Extension `" << extension << "` is not supported but is required.\n";

        allValid &= valid;
    }

    return allValid;
}

vulk::ContextVulkan::QueueFamilyEntry
vulk::ContextVulkan::findQueueFamilies(const vk::PhysicalDevice& physicalDevice) const noexcept
{
    VULK_SCOPED_PROFILER("ContextVulkan::findQueueFamilies()");

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

uint32_t vulk::ContextVulkan::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
    vk::PhysicalDeviceMemoryProperties memoryProperties{m_physicalDevice.getMemoryProperties()};

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw VulkanException("Could not find a suitable memory type");
}

vulk::ContextVulkan::SwapChainSupportDetails
vulk::ContextVulkan::querySwapChainSupport(const vk::PhysicalDevice& device) const noexcept
{
    SwapChainSupportDetails details{device.getSurfaceCapabilitiesKHR(m_surface),  //
                                    device.getSurfaceFormatsKHR(m_surface),       //
                                    device.getSurfacePresentModesKHR(m_surface)};

    return details;
}

vulk::ContextVulkan& vulk::ContextVulkan::getInstance()
{
    assert(s_instance);
    return *s_instance;
}
