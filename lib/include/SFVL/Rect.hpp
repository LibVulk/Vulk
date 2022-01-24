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

#include <SFVL/Vec2.hpp>

#include <ostream>
#include <algorithm>

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
    constexpr Rect(const Vec2<T>& pos, const Vec2<T>& size) : left{pos.x}, top{pos.y}, width{size.x}, height{size.y} {}

    constexpr bool operator==(const Rect<T>& right) const noexcept
    {
        return left == right.left && top == right.top && width == right.width && height == right.height;
    }

    constexpr bool operator!=(const Rect<T>& right) const noexcept
    {
        return left != right.left || top != right.top || width != right.width || height != right.height;
    }

    [[nodiscard]] constexpr bool contains(T x, T y) const noexcept
    {
        const T minX = std::min(left, static_cast<T>(left + width));
        const T maxX = std::max(left, static_cast<T>(left + width));
        const T minY = std::min(top, static_cast<T>(top + height));
        const T maxY = std::max(top, static_cast<T>(top + height));

        return (x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY);
    }

    [[nodiscard]] constexpr bool contains(const Vec2<T>& point) const noexcept
    {
        const T maxX = std::max(left, static_cast<T>(left + width));
        const T minX = std::min(left, static_cast<T>(left + width));
        const T minY = std::min(top, static_cast<T>(top + height));
        const T maxY = std::max(top, static_cast<T>(top + height));

        return (point.x >= minX) && (point.x <= maxX) && (point.y >= minY) && (point.y <= maxY);
    }

    constexpr friend std::ostream& operator<<(std::ostream& os, const Rect<T>& rect) noexcept
    {
        return os << rect.left << ' ' << rect.top << ' ' << rect.width << ' ' << rect.height;
    }
};

using Rectf = Rect<float>;
using Rectd = Rect<double>;
using Recti = Rect<int>;
using Rectu = Rect<unsigned int>;
}  // namespace sfvl
