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

#include "Vulk/Mouse.hpp"

#include <iostream>

std::unordered_map<GLFWwindow*, vulk::Mouse*> vulk::Mouse::s_linkedMouse{};

vulk::Mouse::Mouse(GLFWwindow* window)
{
    assert(window);
    s_linkedMouse[window] = this;
}

vulk::Mouse::~Mouse()
{
}

void vulk::Mouse::onButtonPressed(int button, int action)
{
    if (action == GLFW_PRESS)
    {
        std::cout << "Button pressed !" << std::endl;
        m_mouseInputs[static_cast<size_t>(button)] = true;
    } else if (action == GLFW_RELEASE)
    {
        std::cout << "Button released !" << std::endl;
        m_mouseInputs[static_cast<size_t>(button)] = false;
    }
}
