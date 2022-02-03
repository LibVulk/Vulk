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

#include <iostream>

std::unordered_map<GLFWwindow*, vulk::Keyboard*> vulk::Keyboard::s_linkedKeyboard{};
std::unordered_map<int, bool> vulk::Keyboard::s_keyboardInputs{
  {1, false},   {2, false},   {3, false},   {4, false},   {5, false},   {6, false},   {7, false},   {8, false},
  {9, false},   {10, false},  {11, false},  {12, false},  {13, false},  {14, false},  {15, false},  {16, false},
  {17, false},  {18, false},  {19, false},  {20, false},  {21, false},  {22, false},  {23, false},  {24, false},
  {25, false},  {26, false},  {27, false},  {28, false},  {29, false},  {30, false},  {31, false},  {32, false},
  {33, false},  {34, false},  {35, false},  {36, false},  {37, false},  {38, false},  {39, false},  {40, false},
  {41, false},  {42, false},  {43, false},  {44, false},  {45, false},  {46, false},  {47, false},  {48, false},
  {49, false},  {50, false},  {51, false},  {52, false},  {53, false},  {54, false},  {55, false},  {56, false},
  {57, false},  {58, false},  {59, false},  {60, false},  {61, false},  {62, false},  {63, false},  {64, false},
  {65, false},  {66, false},  {67, false},  {68, false},  {86, false},  {87, false},  {88, false},  {285, false},
  {311, false}, {312, false}, {327, false}, {328, false}, {329, false}, {331, false}, {333, false}, {335, false},
  {336, false}, {337, false}, {339, false}, {347, false},
};

vulk::Keyboard::Keyboard(GLFWwindow* window)
{
    s_linkedKeyboard[window] = this;
}

vulk::Keyboard::~Keyboard()
{
}

void vulk::Keyboard::onKeyPressed(int scancode, int action)
{
    if (action == GLFW_PRESS)
    {
        std::cout << "Key Pressed !" << std::endl;
        if (s_keyboardInputs.find(scancode) != s_keyboardInputs.end())
            s_keyboardInputs[scancode] = true;
    } else if (action == GLFW_RELEASE)
    {
        std::cout << "Key Released !" << std::endl;
        if (s_keyboardInputs.find(scancode) != s_keyboardInputs.end())
            s_keyboardInputs[scancode] = false;
    }
}

bool vulk::Keyboard::isKeyDown(vulk::Key key) noexcept
{
    auto sc = glfwGetKeyScancode(static_cast<int>(key));
    if (s_keyboardInputs.find(sc) != s_keyboardInputs.end())
        return s_keyboardInputs[sc];
    return false;
}
