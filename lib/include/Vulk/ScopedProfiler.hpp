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
#include <utility>

#include "Vulk/Macros.hpp"

namespace vulk::utils {
class ScopedProfiler final
{
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = decltype(Clock::now());
    using DurationSeconds = std::chrono::duration<double>;
    using DurationMillis = std::chrono::duration<double, std::milli>;

    explicit ScopedProfiler(const char* name) noexcept;
    ~ScopedProfiler();

    ScopedProfiler(ScopedProfiler&&) = delete;
    ScopedProfiler(const ScopedProfiler&) = delete;
    ScopedProfiler& operator=(ScopedProfiler&&) = delete;
    ScopedProfiler& operator=(const ScopedProfiler&) = delete;

private:
    const TimePoint m_start;
    const char* const m_name;
};
}  // namespace vulk::utils

#if VULK_WITH_SCOPED_PROFILER
    #define VULK_SCOPED_PROFILER(x) const vulk::utils::ScopedProfiler VULK_STRCAT(_SCOPED_PROFILER_, __LINE__)(x)
#else
    #define VULK_SCOPED_PROFILER(x) (void) 0
#endif
