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

#include <GLFW/glfw3.h>

#include <array>
#include <functional>

namespace vulk {

enum class Buttons
{
    BUTTON_1 = GLFW_MOUSE_BUTTON_1,
    BUTTON_2 = GLFW_MOUSE_BUTTON_2,
    BUTTON_3 = GLFW_MOUSE_BUTTON_3,
    BUTTON_4 = GLFW_MOUSE_BUTTON_4,
    BUTTON_5 = GLFW_MOUSE_BUTTON_5,
    BUTTON_6 = GLFW_MOUSE_BUTTON_6,
    BUTTON_7 = GLFW_MOUSE_BUTTON_7,
    BUTTON_8 = GLFW_MOUSE_BUTTON_8,
    BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
    BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
    BUTTON_LAST = GLFW_MOUSE_BUTTON_LAST,
};

class Mouse final
{
public:
    explicit Mouse(GLFWwindow* window);
    ~Mouse();

    inline bool isButtonDown(vulk::Buttons button) const noexcept { return m_mouseInputs[static_cast<size_t>(button)]; }

    void onButtonPressed(int button, int action);

    [[nodiscard]] inline static Mouse* getMouse(GLFWwindow* window) { return s_linkedMouse[window]; }

private:
    static std::unordered_map<GLFWwindow*, Mouse*> s_linkedMouse;
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseInputs{};
};
}  // namespace vulk
