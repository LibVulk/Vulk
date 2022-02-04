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

#include "Vulk/Window.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Vulk/Contexts/ContextGLFW.hpp"
#include "Vulk/Contexts/ContextVulkan.hpp"
#include "Vulk/Exceptions.hpp"

vulk::Window::Window(unsigned int width, unsigned int height, const char* title)
{
    assert(width != 0);
    assert(height != 0);
    assert(title != nullptr);

    ContextGLFW::ensureInstance();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO: Dynamic from arguments

    m_windowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);

    if (!m_windowHandle)
        throw GLFWException();

    ContextVulkan::createInstance(m_windowHandle);
}

vulk::Window::~Window()
{
    if (m_windowHandle)
        glfwDestroyWindow(m_windowHandle);
}

vulk::Window::Window(Window&& rhs) noexcept : m_windowHandle{rhs.m_windowHandle}
{
    rhs.m_windowHandle = nullptr;
}

vulk::Window& vulk::Window::operator=(Window&& rhs) noexcept
{
    m_windowHandle = rhs.m_windowHandle;
    rhs.m_windowHandle = nullptr;

    return *this;
}

void vulk::Window::display()
{
    ContextVulkan::getInstance().draw();

    m_frameManager.update();
}

void vulk::Window::pollEvents()
{
    glfwPollEvents();
}

void vulk::Window::close() noexcept
{
    glfwSetWindowShouldClose(m_windowHandle, true);
}

bool vulk::Window::isOpen() const noexcept
{
    return !glfwWindowShouldClose(m_windowHandle);
}
