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

#include <cstdint>
#include <vector>

namespace vulk {
class Shader
{
public:
    using Type = vk::ShaderStageFlagBits;

    Shader(vk::Device& device, const char* filePath, Type type);
    ~Shader();

    [[nodiscard]] const vk::PipelineShaderStageCreateInfo& getShaderStageCreateInfo() const noexcept
    {
        return m_pipelineShaderStageCreateInfo;
    }

private:
    vk::Device m_device;  // TODO: Remove once vk::raii is implemented

    std::vector<char> m_buffer{};
    vk::ShaderModule m_shaderModule{};
    vk::PipelineShaderStageCreateInfo m_pipelineShaderStageCreateInfo{};

    Type m_type{};
};
}  // namespace vulk
