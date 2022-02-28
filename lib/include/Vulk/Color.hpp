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

#include <compare>
#include <cstdint>

namespace vulk {
struct Color
{
    uint8_t r{};
    uint8_t g{};
    uint8_t b{};
    uint8_t a{};

    constexpr Color() noexcept = default;
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept : r{r}, g{g}, b{b}, a{a} {}
    constexpr explicit Color(uint32_t value) noexcept
    {
        constexpr uint32_t MASK = 0x000000FF;

        a = value & MASK;
        value >>= 8;
        b = value & MASK;
        value >>= 8;
        g = value & MASK;
        value >>= 8;
        r = static_cast<uint8_t>(value);
    }

    constexpr Color(Color&&) noexcept = default;
    constexpr Color(const Color&) noexcept = default;
    constexpr Color& operator=(Color&&) noexcept = default;
    constexpr Color& operator=(const Color&) noexcept = default;

    constexpr auto operator<=>(const Color&) const noexcept = default;

    [[nodiscard]] constexpr uint32_t value() const noexcept
    {
        uint32_t value = r;

        value <<= 8;
        value += g;
        value <<= 8;
        value += b;
        value <<= 8;
        value += a;

        return value;
    }

    static Color Red;
    static Color Green;
    static Color Blue;
    static Color Magenta;
    static Color Cyan;
    static Color Yellow;
    static Color Black;
    static Color White;
    static Color Grey;
    static Color Transparent;
    static Color CoolYellow;
};
}  // namespace vulk
