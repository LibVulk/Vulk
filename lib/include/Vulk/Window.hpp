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

#include <memory>

#include "FrameManager.hpp"
#include "Keyboard.hpp"

struct GLFWwindow;

namespace vulk {
class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    Window(Window&& rhs) noexcept;
    Window& operator=(Window&& rhs) noexcept;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void display();
    void pollEvents();
    void close() noexcept;

    [[nodiscard]] bool isOpen() const noexcept;
    [[nodiscard]] FrameManager& getFrameManager() noexcept { return m_frameManager; }
    [[nodiscard]] const FrameManager& getFrameManager() const noexcept { return m_frameManager; }

private:
    GLFWwindow* m_windowHandle{nullptr};
    FrameManager m_frameManager{};

    std::unique_ptr<Keyboard> m_keyboard{nullptr};
};
}  // namespace vulk
