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

#include "Vulk/Contexts/ContextGLFW.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Vulk/Error.hpp"

std::unique_ptr<vulk::ContextGLFW> vulk::ContextGLFW::s_instance{nullptr};

vulk::ContextGLFW::ContextGLFW()
{
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error(utils::getGLFWError());
}

vulk::ContextGLFW::~ContextGLFW()
{
    glfwTerminate();
}

vulk::ContextGLFW& vulk::ContextGLFW::getInstance()
{
    if (!s_instance)
        s_instance = std::unique_ptr<ContextGLFW>{new ContextGLFW};
    return *s_instance;
}
