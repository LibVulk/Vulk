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

#include <array>
#include <chrono>
#include <functional>
#include <optional>
#include <string>

#include "Time.hpp"

namespace vulk {
/**
 * Class that manages a window's framerate, delta time (or frame time) and other time/frame logics
 */
class FrameManager final
{
public:
    /**
     * OnSecond event callback signature
     */
    using OnSecondCallback = std::function<void(const FrameManager& frameManager)>;

    /**
     * Static string buffer type for FPS counter as a string
     */
    using FramerateStringBuffer = std::array<char, 16>;

    FrameManager();

    /**
     * Updates the frame manager. Call ONCE every frame. Already done in the Window class
     */
    void update() noexcept;

    /**
     * Add an optional callback to be called once every second
     * @param func Function callback to use. std::nullopt to reset
     */
    void setOnSecondCallback(const OnSecondCallback& func) noexcept { m_onSecondCallback = func; }

    /**
     * Returns the frame time of the last update. Use this for framerate independent operations
     * @return Frame time in seconds
     */
    [[nodiscard]] auto getDeltaTime() const noexcept { return m_deltaTime; }

    /**
     * Returns the current framerate
     * @return Frames per seconds unsigned integer
     */
    [[nodiscard]] auto getFPS() const noexcept { return m_framerate; }

    /**
     * Returns the framerate as a string buffer
     * @return Framerate string buffer
     */
    [[nodiscard]] FramerateStringBuffer getFramerateStringBuffer() const noexcept;

    /**
     * Returns the framerate as a std::string
     * @return Framerate as a string "XXX fps"
     */
    [[nodiscard]] std::string getFramerateString() const noexcept;

private:
    Duration m_duration{};
    TimePoint m_lastFrame{};
    TimePoint m_lastSecond{};

    float m_deltaTime{};
    uint32_t m_framerate{};
    uint32_t m_frameCounter{};

    std::optional<OnSecondCallback> m_onSecondCallback{std::nullopt};
};
}  // namespace vulk

std::ostream& operator<<(std::ostream& os, const vulk::FrameManager& frameManager);
