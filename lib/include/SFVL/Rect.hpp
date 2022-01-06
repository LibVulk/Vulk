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

#include <ostream>

#include <Vec2.hpp>

namespace sfvl {
	template<typename T>

struct Rect
{
    T left;
    T top;
    T width;
    T height;

    constexpr Rect() = default;
    constexpr Rect(T aLeft, T aTop, T aWidth, T aHeight) : left{aLeft}, top{aTop}, width{aWidth}, height{aHeight} {}
    constexpr Rect(const Vec2& pos, const Vec2& size) : left{pos.x}, top{pos.y}, width{size.x}, height{size.y} {}

    constexpr bool operator==(const Rect& right)
    {
        return left == right.left && top == right.top && width == right.width && height == right.height; 
    }

    constexpr bool operator!=(const Rect& right)
    {
        return left != right.left || top != right.top || width != right.width || height != right.height;
    }

    [[nodiscard]] constexpr static bool contains(T x, T y)
    { 
        if (left < x && top < y)
            return true;
        return false;
    } const noexcept

    [[nodiscard]] constexpr static bool contains(const Vec2& point)
    {
        if (left < point.x && top < point.y)
            return true;
        return false;            
    } const noexcept

    constexpr friend std::ostream& operator<<(std::ostream& os, const Rect& rect) noexcept
    {
        return os << rect.left << ' ' << rect.top << ' ' << rect.width << ' ' << rect.height;
    }
};

using FloatRect = Rect<float>;
using DoubleRect = Rect<double>;
using IntRect = Rect<int>;
using UnsignedRect = Rect<unsigned int>;
} // namespace sfvl