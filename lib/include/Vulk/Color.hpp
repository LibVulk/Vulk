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
/**
 * The color structure can be used to give a color to your objects, a tint to textures or any other color related stuff
 */
struct Color
{
    uint8_t r{};
    uint8_t g{};
    uint8_t b{};
    uint8_t a{};

    /**
     * Constructs a default color (Black transparent, 0x00000000)
     */
    constexpr Color() noexcept = default;

    /**
     * Constructs a color using rgba components, [0;255]
     * @param _r Red component
     * @param _g Green component
     * @param _b Blue component
     * @param _a Alpha component (opacity)
     */
    constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) noexcept : r{_r}, g{_g}, b{_b}, a{_a} {}

    /**
     * Constructs a color from its hexadecimal value
     * @param value Hexadecimal value, format 0xRRGGBBAA with RR=Red, GG=Green, BB=Blue and AA=Alpha
     */
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

    /**
     * Converts RGBA components to the integer value
     * @return 32 unsigned bit integer value representing the color
     */
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
    static Color Transparent;  // Black with 0 opacity
    static Color CoolYellow;   // Just a cool yellow I like :3
};
}  // namespace vulk
