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

#include "Vulk/Keyboard.hpp"

#include <cassert>
#include <iostream>

std::unordered_map<GLFWwindow*, vulk::Keyboard*> vulk::Keyboard::s_linkedKeyboard{};

vulk::Keyboard::Keyboard(GLFWwindow* window)
{
    assert(window);
    s_linkedKeyboard[window] = this;
}

vulk::Keyboard::~Keyboard()
{
}

void vulk::Keyboard::onKeyPressed(int scancode, int action)
{
    if (action == GLFW_PRESS)
    {
        std::cout << "Key Pressed ! " << std::endl;
        m_keyboardInputs[static_cast<size_t>(scancode)] = true;
    } else if (action == GLFW_RELEASE)
    {
        std::cout << "Key Released !" << std::endl;
        m_keyboardInputs[static_cast<size_t>(scancode)] = false;
    }
}
