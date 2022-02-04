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

#include <stdexcept>

#include "Vulk/ClassUtils.hpp"

namespace vk {
enum class Result;

template<typename T>
struct ResultValue;
}  // namespace vk

namespace vulk {
class Exception : public std::exception
{
public:
    Exception() : Exception{"No additional details provided", "vulk::Exception"} {}

    explicit Exception(const char* message, const char* category = "vulk::Exception")
        : m_what{std::string{category} + ": " + message}
    {
    }

    explicit Exception(const std::string& message, const std::string& category = "vulk::Exception")
        : m_what{category + ": " + message}
    {
    }

    VULK_DEFAULT_MOVE_AND_COPY(Exception)

    [[nodiscard]] const char* what() const noexcept override { return m_what.c_str(); }

protected:
    std::string m_what;
};

#define VULK_DEFINE_EXCEPTION(ClassName, OverriddenClassName)                                    \
    class ClassName : public OverriddenClassName                                                 \
    {                                                                                            \
    public:                                                                                      \
        ClassName() : OverriddenClassName{"No additional details provided", #ClassName} {}       \
        explicit ClassName(const char* message, const char* category = #ClassName)               \
            : OverriddenClassName{message, category}                                             \
        {                                                                                        \
        }                                                                                        \
        explicit ClassName(const std::string& message, const std::string& category = #ClassName) \
            : OverriddenClassName{message, category}                                             \
        {                                                                                        \
        }                                                                                        \
    };

VULK_DEFINE_EXCEPTION(IOException, Exception)
VULK_DEFINE_EXCEPTION(FileNotFoundException, IOException)
VULK_DEFINE_EXCEPTION(PermissionDeniedException, IOException)

VULK_DEFINE_EXCEPTION(ThirdPartyException, Exception)

class GLFWException : public ThirdPartyException
{
public:
    explicit GLFWException(const std::string& message = getGLFWError()) : ThirdPartyException(message, "GLFWException")
    {
    }

    static std::string getGLFWError() noexcept;
};

class VulkanException : public ThirdPartyException
{
public:
    explicit VulkanException(vk::Result result);
    explicit VulkanException(const char* what);
    explicit VulkanException(const std::string& what);
};

inline vk::Result handleVulkanError(vk::Result res)
{
    if (res != vk::Result::eSuccess)
        throw VulkanException(res);
    return res;
}

inline vk::Result handleVulkanError(VkResult vkRes)
{
    auto res = static_cast<vk::Result>(vkRes);

    if (res != vk::Result::eSuccess)
        throw VulkanException(res);
    return res;
}

template<typename T>
T& handleVulkanError(vk::ResultValue<T>& res)
{
    if (res.result != vk::Result::eSuccess)
        throw VulkanException(res.result);
    return res.value;
}

template<typename T>
const T& handleVulkanError(const vk::ResultValue<T>& res)
{
    if (res.result != vk::Result::eSuccess)
        throw VulkanException(res.result);
    return res.value;
}
}  // namespace vulk
