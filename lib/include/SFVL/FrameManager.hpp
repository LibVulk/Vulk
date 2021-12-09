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

#include <chrono>
#include <functional>
#include <optional>
#include <string>

#include "Time.hpp"

namespace sfvl {
class FrameManager final
{
public:
    using OnSecondCallback = std::function<void(const FrameManager& frameManager)>;

    FrameManager();

    void update() noexcept;

    void setOnSecondCallback(const OnSecondCallback& func) noexcept { m_onSecondCallback = func; }

    [[nodiscard]] auto getDeltaTime() const noexcept { return m_deltaTime; }
    [[nodiscard]] auto getFramerate() const noexcept { return m_framerate; }
    [[nodiscard]] std::string getFramerateString() const noexcept;

private:
    Duration m_duration{};
    TimePoint m_lastFrame{};
    TimePoint m_lastSecond{};

    double m_deltaTime{};
    uint32_t m_framerate{};
    uint32_t m_frameCounter{};

    std::optional<OnSecondCallback> m_onSecondCallback{std::nullopt};
};
}  // namespace sfvl
