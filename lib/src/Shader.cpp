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

#include "Shader.hpp"

#include <iostream>

#include "ScopedProfiler.hpp"
#include "Utils.hpp"

sfvl::Shader::Shader(vk::Device& device, const char* filePath, sfvl::Shader::Type type)
    : m_device{device}, m_buffer{sfvl::utils::fileToBinary(filePath)}, m_type{type}
{
    SFVL_SCOPED_PROFILER("Shader::Shader()");

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.codeSize = m_buffer.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<decltype(shaderModuleCreateInfo.pCode)>(m_buffer.data());

    m_shaderModule = device.createShaderModule(shaderModuleCreateInfo);

    if (!m_shaderModule)
        throw std::runtime_error("Unable to create Vulkan Shader module");

    m_pipelineShaderStageCreateInfo.stage = type;
    m_pipelineShaderStageCreateInfo.module = m_shaderModule;
    m_pipelineShaderStageCreateInfo.pName = "main";
}

sfvl::Shader::~Shader()
{
    if (m_device && m_shaderModule)
        m_device.destroy(m_shaderModule);
}
