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

#include <iostream>
#include <set>
#include <string_view>

#include "Vulk/Exceptions.hpp"
#include "Vulk/ScopedProfiler.hpp"
#include "Vulk/Shader.hpp"
#include "Vulk/Utils.hpp"
#include "Vulk/Vertex.hpp"

const size_t vulk::ContextVulkan::s_maxFramesInFlight{2};
std::unique_ptr<vulk::ContextVulkan> vulk::ContextVulkan::s_instance{nullptr};

vulk::ContextVulkan::ContextVulkan(GLFWwindow* windowHandle)
{
    VULK_SCOPED_PROFILER("ContextVulkan::ContextVulkan()");

    assert(windowHandle);

#if VULK_DEBUG
    // printAvailableValidationLayers();
    verifyValidationLayersSupport();
#endif

    createInstance();
    createSurface(windowHandle);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain(windowHandle);
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
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

        for (auto& frameSemaphore : m_frameSyncObjects)
            frameSemaphore.destroy(m_device);

        m_device.destroy(m_commandPool);

        for (auto& framebuffer : m_swapChainFrameBuffers)
            m_device.destroy(framebuffer);

        m_device.destroy(m_pipeline);
        m_device.destroy(m_pipelineLayout);
        m_device.destroy(m_renderPass);

        for (auto& imageView : m_swapChainImageViews)
            m_device.destroy(imageView);

        m_device.destroy(m_swapChain);
    }

    m_instance.destroy(m_surface);
    m_device.destroy();
    m_instance.destroy();
}

void vulk::ContextVulkan::createInstance(GLFWwindow* windowHandle)
{
    if (s_instance)
        return;

    s_instance = std::unique_ptr<ContextVulkan>(new ContextVulkan{windowHandle});
    assert(s_instance);
}

void vulk::ContextVulkan::draw()
{
    static constexpr auto NO_TIMEOUT = std::numeric_limits<uint64_t>::max();

    // Wait until the frame is released

    const auto* fence = &m_frameSyncObjects[m_currentFrame].fence;

    handleVulkanError(m_device.waitForFences(1, fence, true, NO_TIMEOUT));

    const auto& imageIndex = handleVulkanError(
      m_device.acquireNextImageKHR(m_swapChain, NO_TIMEOUT, m_frameSyncObjects[m_currentFrame].imageAvailable));

    if (m_imagesInFlight[imageIndex])
        (void) m_device.waitForFences(1, &m_imagesInFlight[imageIndex], true, NO_TIMEOUT);
    m_imagesInFlight[imageIndex] = m_imagesInFlight[m_currentFrame];

    const std::array swapChains{m_swapChain};
    const std::array waitSemaphores{m_frameSyncObjects[m_currentFrame].imageAvailable};
    const std::array signalSemaphores{m_frameSyncObjects[m_currentFrame].renderFinished};
    const vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    handleVulkanError(m_device.resetFences(1, fence));
    handleVulkanError(m_graphicsQueue.submit(1, &submitInfo, m_frameSyncObjects[m_currentFrame].fence));

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    presentInfo.pWaitSemaphores = signalSemaphores.data();
    presentInfo.swapchainCount = static_cast<uint32_t>(swapChains.size());
    presentInfo.pSwapchains = swapChains.data();
    presentInfo.pImageIndices = &imageIndex;
    // presentInfo.pResults = nullptr; // would be useful if ever adding more swap chains

    (void) m_presentQueue.presentKHR(presentInfo);  // TODO: handle error?

    m_currentFrame = (m_currentFrame + 1) % s_maxFramesInFlight;
}

void vulk::ContextVulkan::printAvailableValidationLayers()
{
    std::cout << "Available Layers:\n";

    for (const auto& layer : getAvailableValidationLayers())
    {
        std::cout << "\t" << layer.layerName << '\n';
    }
    std::cout.flush();
}

void vulk::ContextVulkan::verifyValidationLayersSupport()
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
            throw VulkanException("Validation layer `" + std::string(layerName) + "` is not supported");
        }
    }
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
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYER_NAMES.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYER_NAMES.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    handleVulkanError(vk::createInstance(&createInfo, nullptr, &m_instance));
}

void vulk::ContextVulkan::createSurface(GLFWwindow* windowHandle)
{
    VULK_SCOPED_PROFILER("ContextVulkan::createSurface()");

    assert(windowHandle);

    VkSurfaceKHR surface;

    handleVulkanError(glfwCreateWindowSurface(m_instance, windowHandle, nullptr, &surface));

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
            m_swapChainSupport = swapChainSupportDetails;
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

void vulk::ContextVulkan::createSwapChain(GLFWwindow* windowHandle)
{
    VULK_SCOPED_PROFILER("ContextVulkan::createSwapChain()");

    assert(windowHandle);

    chooseSwapSurfaceFormat();
    chooseSwapPresentMode();
    chooseSwapExtent(windowHandle);

    uint32_t imageCount = m_swapChainSupport.capabilities.minImageCount + 1;
    if (m_swapChainSupport.capabilities.maxImageCount > 0 && imageCount > m_swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = m_swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_surfaceFormat.format;
    createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;  // vk::ImageUsageFlagBits::eTransferDst later?
    createInfo.preTransform = m_swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;  // customizable later?
    createInfo.presentMode = m_presentMode;
    createInfo.clipped = true;

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

    handleVulkanError(m_device.createSwapchainKHR(&createInfo, nullptr, &m_swapChain));

    m_swapChainImages = m_device.getSwapchainImagesKHR(m_swapChain);
    m_swapChainFormat = m_surfaceFormat.format;
}

void vulk::ContextVulkan::createImageViews()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createImageViews()");

    const auto size = m_swapChainImages.size();
    m_swapChainImageViews.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        vk::ImageViewCreateInfo createInfo{};

        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_swapChainFormat;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        m_swapChainImageViews.push_back(m_device.createImageView(createInfo, nullptr));
    }
}

void vulk::ContextVulkan::createRenderPass()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createRenderPass()");

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainFormat;
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
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = false;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    // temporary manual switch to test things, should be customizable at runtime later
    static constexpr bool tmpTestEnabled = false;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    if constexpr (tmpTestEnabled)  // TODO: investigate and decide what to do
    {
        colorBlendAttachment.blendEnable = true;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
    } else
    {
        colorBlendAttachment.blendEnable = false;
    }

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = tmpTestEnabled;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::array dynamicStatesArray = {vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStatesArray.size());
    dynamicState.pDynamicStates = dynamicStatesArray.data();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};  // empty for now

    handleVulkanError(m_device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
    assert(m_pipelineLayout);

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
    assert(m_pipelineLayout);
}

void vulk::ContextVulkan::createFrameBuffers()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createFrameBuffers()");

    m_swapChainFrameBuffers.reserve(m_swapChainImageViews.size());

    for (const auto& imageView : m_swapChainImageViews)
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

        m_swapChainFrameBuffers.push_back(framebuffer);
    }
}

void vulk::ContextVulkan::createCommandPool()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createCommandPool()");

    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();
    // commandPoolCreateInfo.flags = ??; // TODO: may need to change this one later

    handleVulkanError(m_device.createCommandPool(&commandPoolCreateInfo, nullptr, &m_commandPool));
}

void vulk::ContextVulkan::createCommandBuffers()
{
    VULK_SCOPED_PROFILER("ContextVulkan::createCommandBuffers()");

    m_commandBuffers.resize(m_swapChainFrameBuffers.size());

    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(m_swapChainFrameBuffers.size());

    handleVulkanError(m_device.allocateCommandBuffers(&allocateInfo, m_commandBuffers.data()));

    for (size_t i = 0; i < m_swapChainFrameBuffers.size(); ++i)
    {
        vk::CommandBufferBeginInfo beginInfo{};
        // beginInfo.flags = vk::CommandBufferUsageFlagBits::...;
        // beginInfo.pInheritanceInfo = nullptr;

        handleVulkanError(m_commandBuffers[i].begin(&beginInfo));

        vk::ClearValue clearValue{};
        clearValue.color = vk::ClearColorValue{std::array{0.f, 0.f, 0.f, 1.f}};

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = m_swapChainFrameBuffers[i];
        renderPassBeginInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassBeginInfo.renderArea.extent = m_extent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        m_commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        m_commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        m_commandBuffers[i].draw(3, 1, 0, 0);
        m_commandBuffers[i].endRenderPass();
        m_commandBuffers[i].end();
    }
}

void vulk::ContextVulkan::createSyncObject()
{
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};

    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    m_imagesInFlight.resize(m_swapChainImages.size());
    m_frameSyncObjects.resize(s_maxFramesInFlight);

    for (auto& frameSyncObj : m_frameSyncObjects)
    {
        handleVulkanError(m_device.createSemaphore(&semaphoreInfo, nullptr, &frameSyncObj.imageAvailable));
        handleVulkanError(m_device.createSemaphore(&semaphoreInfo, nullptr, &frameSyncObj.renderFinished));
        handleVulkanError(m_device.createFence(&fenceInfo, nullptr, &frameSyncObj.fence));
    }
}

void vulk::ContextVulkan::chooseSwapSurfaceFormat()
{
    assert(!m_swapChainSupport.formats.empty());

    for (const auto& format : m_swapChainSupport.formats)
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
    m_surfaceFormat = m_swapChainSupport.formats[0];
}

void vulk::ContextVulkan::chooseSwapPresentMode()
{
    for (const auto& presentMode : PRESENT_MODES_PREFERRED)
    {
        // This can be expensive to search the list again and again,
        // but since the list is less than 5 elements and is done once, shouldn't matter too much.

        if (utils::vectorContains(m_swapChainSupport.presentModes, presentMode))
        {
            m_presentMode = presentMode;
            return;
        }
    }

    // *Highly* unlikely
    throw std::runtime_error("No present mode available.");
}

void vulk::ContextVulkan::chooseSwapExtent(GLFWwindow* windowHandle)
{
    assert(windowHandle);

    const auto& caps = m_swapChainSupport.capabilities;

    if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_extent = caps.currentExtent;
    } else
    {
        int width, height;

        glfwGetFramebufferSize(windowHandle, &width, &height);

        m_extent = vk::Extent2D{
          std::clamp(static_cast<uint32_t>(width), caps.minImageExtent.width, caps.maxImageExtent.width),
          std::clamp(static_cast<uint32_t>(height), caps.minImageExtent.height, caps.maxImageExtent.height)};
    }
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
