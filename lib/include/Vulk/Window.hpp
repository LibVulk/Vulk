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
#include "Mouse.hpp"

struct GLFWwindow;

namespace vulk {
class Window
{
public:
    /**
    * @brief Construct a new Window object
    *
    * @param width
    * @param height
    * @param title
    */
    Window(unsigned int width, unsigned int height, const char* title);
    /**
     * @brief Destroy the Window object
     *
     */
    ~Window();

    /**
     * @brief Construct a new Window object
     *
     * @param rhs
     */
    Window(Window&& rhs) noexcept;
    /**
     * @brief
     *
     * @param rhs
     * @return Window&
     */
    Window& operator=(Window&& rhs) noexcept;

    /**
     * @brief Construct a new Window object
     *
     */
    Window(const Window&) = delete;
    /**
     * @brief
     *
     * @return Window&
     */
    Window& operator=(const Window&) = delete;

    /**
     * @brief
     *
     */
    void display();
    /**
     * @brief
     *
     */
    void pollEvents();
    /**
     * @brief
     *
     */
    void close() noexcept;

    /**
     * @brief Changes the title of the window.
     *
     * @param newTitle The new window title, ignored if null.
     * @note This will be updated on the next pollEvents() call.
     */
    void setTitle(const char* newTitle) noexcept;

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool isOpen() const noexcept;
    /**
     * @brief Get the Frame Manager object
     *
     * @return FrameManager&
     */
    [[nodiscard]] FrameManager& getFrameManager() noexcept { return m_frameManager; }
    /**
     * @brief Get the Frame Manager object
     *
     * @return const FrameManager&
     */
    [[nodiscard]] const FrameManager& getFrameManager() const noexcept { return m_frameManager; }

private:
    /**
     * @brief
     *
     */
    GLFWwindow* m_windowHandle{nullptr};
    /**
     * @brief
     *
     */
    FrameManager m_frameManager{};

    /**
     * @brief
     *
     */
    std::unique_ptr<Keyboard> m_keyboard{nullptr};
    /**
     * @brief
     *
     */
    std::unique_ptr<Mouse> m_mouse{nullptr};
};
}  // namespace vulk
