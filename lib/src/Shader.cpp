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

#include "Vulk/Shader.hpp"

#include <iostream>

#include "Vulk/Exceptions.hpp"
#include "Vulk/ScopedProfiler.hpp"
#include "Vulk/Utils.hpp"

vulk::Shader::Shader(vk::Device& device, const char* filePath, vulk::Shader::Type type)
    : m_device{device}, m_buffer{vulk::utils::fileToBinary(filePath)}, m_type{type}
{
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.codeSize = m_buffer.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<decltype(shaderModuleCreateInfo.pCode)>(m_buffer.data());

    handleVulkanError(device.createShaderModule(&shaderModuleCreateInfo, nullptr, &m_shaderModule));
    assert(m_shaderModule);

    m_pipelineShaderStageCreateInfo.stage = type;
    m_pipelineShaderStageCreateInfo.module = m_shaderModule;
    m_pipelineShaderStageCreateInfo.pName = "main";
}

vulk::Shader::~Shader()
{
    if (m_device && m_shaderModule)
        m_device.destroy(m_shaderModule);
}
