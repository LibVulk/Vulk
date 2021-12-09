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

#include "Window.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Contexts/ContextGLFW.hpp"
#include "Contexts/ContextVulkan.hpp"
#include "Error.hpp"

sfvl::Window::Window(int width, int height, const char* title)
{
    ContextGLFW::ensureInstance();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO: Dynamic from arguments

    m_windowHandle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!m_windowHandle)
        throw std::runtime_error(utils::getGLFWError());

    ContextVulkan::createInstance(m_windowHandle);
}

sfvl::Window::~Window()
{
    if (m_windowHandle)
        glfwDestroyWindow(m_windowHandle);
}

sfvl::Window::Window(Window&& rhs) noexcept : m_windowHandle{rhs.m_windowHandle}
{
    rhs.m_windowHandle = nullptr;
}

sfvl::Window& sfvl::Window::operator=(Window&& rhs) noexcept
{
    m_windowHandle = rhs.m_windowHandle;
    rhs.m_windowHandle = nullptr;

    return *this;
}

void sfvl::Window::display()
{
    m_frameManager.update();
}

void sfvl::Window::pollEvents()
{
    glfwPollEvents();
}

void sfvl::Window::close() noexcept
{
    glfwSetWindowShouldClose(m_windowHandle, true);
}

bool sfvl::Window::isOpen() const noexcept
{
    return !glfwWindowShouldClose(m_windowHandle);
}
