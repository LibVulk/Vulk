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
/**
 * @brief
 * The Vec3 structure that can be use to create objects or do mathematical operations
 *
 */
struct Vec3
{
    T x;
    T y;
    T z;

    /**
     * @brief
     * The default constructor
     */
    constexpr Vec3() = default;
    /**
     * @brief
     * Vec3 constructor with three different parameters
     * @param aX x value of the vector
     * @param aY y value of the vector
     * @param aZ z value of the vector
     */
    constexpr Vec3(T aX, T aY, T aZ) : x{aX}, y{aY}, z{aZ} {}
    /**
     * @brief
     * Vec3 constructor with one parameter
     * @param value x, y and z will take this value
     */
    constexpr explicit Vec3(T value) : x{value}, y{value}, z{value} {}

    /**
     * @brief
     * Return the negative vector
     * @return constexpr Vec3
     */
    constexpr Vec3 operator-() const noexcept { return Vec3{-x, -y, -z}; }

    /**
     * @brief
     * Addition betwen two Vec3
     * @param rhs
     * @return constexpr Vec3&
     */
    constexpr Vec3& operator+=(const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    /**
     * @brief
     * Substraction between two Vec3
     * @param rhs
     * @return constexpr Vec3&
     */
    constexpr Vec3& operator-=(const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    /**
     * @brief
     * Multiplication between a Vec3 and a value
     * @param value
     * @return constexpr Vec3&
     */
    constexpr Vec3& operator*=(const T value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    /**
     * @brief
     * Division between a Vec3 and a value
     * @param value
     * @return constexpr Vec3&
     */
    constexpr Vec3& operator/=(const T value) { return *this *= (static_cast<T>(1) / value); }

    /**
     * @brief
     * Return the dot product of two Vec3
     * @param lhs
     * @param rhs
     * @return constexpr T
     */
    [[nodiscard]] constexpr static T dot(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    /**
     * @brief
     * Return the cross product of two vectors
     * @param lhs
     * @param rhs
     * @return constexpr Vec3
     */
    [[nodiscard]] constexpr static Vec3 cross(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3{lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x};
    }

    /**
     * @brief
     * Return the length squared of the Vec3
     * @return constexpr T
     */
    [[nodiscard]] constexpr T lengthSquared() const noexcept { return x * x + y * y + z * z; }
    /**
     * @brief
     * Return the length of the Vec3
     * @return constexpr T
     */
    [[nodiscard]] constexpr T length() const noexcept { return static_cast<T>(std::sqrt(lengthSquared())); }
    /**
     * @brief
     * Return the normalized version of the vector
     * @return constexpr Vec3
     */
    [[nodiscard]] constexpr Vec3 normalized() const noexcept { return *this / length(); }

    /**
     * @brief
     *
     * @param os
     * @param vec3
     * @return constexpr std::ostream&
     */
    constexpr friend std::ostream& operator<<(std::ostream& os, const Vec3& vec3) noexcept
    {
        return os << vec3.x << ' ' << vec3.y << ' ' << vec3.z;
    }
};

template<typename T>
constexpr Vec3<T> operator+(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept
{
    return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

template<typename T>
constexpr Vec3<T> operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept
{
    return Vec3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

template<typename T>
constexpr Vec3<T> operator*(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept
{
    return Vec3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

template<typename T>
constexpr Vec3<T> operator*(const Vec3<T>& lhs, T t) noexcept
{
    return Vec3{lhs.x * t, lhs.y * t, lhs.z * t};
}

template<typename T>
constexpr Vec3<T> operator*(T t, const Vec3<T>& rhs) noexcept
{
    return rhs * t;
}

template<typename T>
constexpr Vec3<T> operator/(const Vec3<T>& lhs, T t) noexcept
{
    return (1 / t) * lhs;
}

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<unsigned int>;
}  // namespace vulk
