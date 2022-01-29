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

#include "FrameManager.hpp"

#include <sstream>

using namespace std::chrono_literals;

vulk::FrameManager::FrameManager() : m_lastFrame{Clock::now()}
{
}

void vulk::FrameManager::update() noexcept
{
    const auto timePoint = Clock::now();

    // Compute frame time
    m_duration = timePoint - m_lastFrame;
    m_deltaTime = m_duration.count();
    m_lastFrame = timePoint;

    // Compute frame per seconds
    m_duration = timePoint - m_lastSecond;

    ++m_frameCounter;
    if (m_duration >= 1s)
    {
        m_framerate = m_frameCounter;
        m_frameCounter = 0;
        m_lastSecond = timePoint;

        if (m_onSecondCallback.has_value())
            m_onSecondCallback.value()(*this);
    }
}

// TODO: There is probably a more optimal way of doing this
std::string vulk::FrameManager::getFramerateString() const noexcept
{
    static constexpr size_t BufferSize = 16;
    char buffer[BufferSize]{};

    snprintf(buffer, BufferSize, "%u fps", m_framerate);
    return std::string{buffer};
}

std::ostream& operator<<(std::ostream& os, const vulk::FrameManager& frameManager)
{
    return os << frameManager.getFramerate() << " fps";
}
