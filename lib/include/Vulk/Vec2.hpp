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

#include <cmath>
#include <ostream>

namespace vulk {
template<typename T>
struct Vec2
{
    T x;
    T y;

    /**
     * @brief
     *
     */
    constexpr Vec2() = default;
    /**
     * @brief
     *
     */
    constexpr Vec2(T aX, T aY) : x{aX}, y{aY} {}
    /**
     * @brief
     *
     */
    constexpr explicit Vec2(T value) : x{value}, y{value} {}

    /**
     * @brief
     *
     * @return constexpr Vec2
     */
    constexpr Vec2 operator-() const noexcept { return Vec2{-x, -y}; }

    /**
     * @brief
     *
     * @param rhs
     * @return constexpr Vec2&
     */
    constexpr Vec2& operator+=(const Vec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    /**
     * @brief
     *
     * @param rhs
     * @return constexpr Vec2&
     */
    constexpr Vec2& operator-=(const Vec2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    /**
     * @brief
     *
     * @param value
     * @return constexpr Vec2&
     */
    constexpr Vec2& operator*=(const T value)
    {
        x *= value;
        y *= value;
        return *this;
    }

    /**
     * @brief
     *
     * @param value
     * @return constexpr Vec2&
     */
    constexpr Vec2& operator/=(const T value) { return *this *= (static_cast<T>(1) / value); }

    /**
     * @brief
     *
     * @param lhs
     * @param rhs
     * @return constexpr T
     */
    [[nodiscard]] constexpr static T dot(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    /**
     * @brief
     *
     * @param lhs
     * @param rhs
     * @return constexpr T
     */
    [[nodiscard]] constexpr static T cross(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

    /**
     * @brief
     *
     * @return constexpr T
     */
    [[nodiscard]] constexpr T lengthSquared() const noexcept { return x * x + y * y; }
    /**
     * @brief
     *
     * @return constexpr T
     */
    [[nodiscard]] constexpr T length() const noexcept { return static_cast<T>(std::sqrt(lengthSquared())); }
    /**
     * @brief
     *
     * @return constexpr Vec2
     */
    [[nodiscard]] constexpr Vec2 normalized() const noexcept { return *this / length(); }

    /**
     * @brief
     *
     * @param os
     * @param Vec2
     * @return constexpr std::ostream&
     */
    constexpr friend std::ostream& operator<<(std::ostream& os, const Vec2& Vec2) noexcept
    {
        return os << Vec2.x << ' ' << Vec2.y << ' ';
    }
};

template<typename T>
constexpr Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
{
    return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

template<typename T>
constexpr Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
{
    return Vec2{lhs.x - rhs.x, lhs.y - rhs.y};
}

template<typename T>
constexpr Vec2<T> operator*(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept
{
    return Vec2{lhs.x * rhs.x, lhs.y * rhs.y};
}

template<typename T>
constexpr Vec2<T> operator*(const Vec2<T>& lhs, T t) noexcept
{
    return Vec2{lhs.x * t, lhs.y * t};
}

template<typename T>
constexpr Vec2<T> operator*(T t, const Vec2<T>& rhs) noexcept
{
    return rhs * t;
}

template<typename T>
constexpr Vec2<T> operator/(const Vec2<T>& lhs, T t) noexcept
{
    return (1 / t) * lhs;
}

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;
}  // namespace vulk
