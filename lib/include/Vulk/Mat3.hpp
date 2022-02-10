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

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cmath>
#include <iostream>

namespace vulk {
template<typename T>
struct Mat3
{
    std::array<T, 9> m{
      1.0, 0.0, 0.0,  // a b c
      0.0, 1.0, 0.0,  // d e f
      0.0, 0.0, 1.0   // g h i
    };

    constexpr Mat3() = default;
    constexpr Mat3(T value) : m{std::array<T, 9>{value, 0.0, 0.0, 0.0, value, 0.0, 0.0, 0.0, value}} {}
    constexpr Mat3(const std::array<T, 9> mat) : m{mat} {}
    constexpr Mat3(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8, T a9)
        : m{std::array<T, 9>{a1, a2, a3, a4, a5, a6, a7, a8, a9}}
    {
    }

    constexpr Mat3 inverse() const noexcept
    {
        auto cfMatrix = coFactor();
        auto d = determinant();

        return Mat3{cfMatrix.m[0] / d, cfMatrix.m[1] / d, cfMatrix.m[2] / d, cfMatrix.m[3] / d, cfMatrix.m[4] / d,
                    cfMatrix.m[5] / d, cfMatrix.m[6] / d, cfMatrix.m[7] / d, cfMatrix.m[8] / d};
    }

    constexpr Mat3 coFactor() const noexcept
    {
        return Mat3{
          ((m[4] * m[8]) - (m[5] * m[7])),  -((m[3] * m[8]) - (m[5] * m[6])), ((m[3] * m[7]) - (m[4] * m[6])),
          -((m[1] * m[8]) - (m[2] * m[7])), ((m[0] * m[8]) - (m[2] * m[6])),  -((m[0] * m[7]) - (m[1] * m[6])),
          ((m[1] * m[5]) - (m[2] * m[4])),  -((m[0] * m[5]) - (m[2] * m[3])), ((m[0] * m[4]) - (m[1] * m[3]))};
    }

    constexpr T determinant() const noexcept
    {
        auto cfMatrix = coFactor();
        return ((m[0] * cfMatrix.m[0]) + (m[1] * cfMatrix.m[1]) + (m[2] * cfMatrix.m[2]));
    }

    constexpr friend std::ostream& operator<<(std::ostream& os, const Mat3& mat3) noexcept
    {
        return os << mat3.m[0] << ' ' << mat3.m[1] << ' ' << mat3.m[2] << '\n'
                  << mat3.m[3] << ' ' << mat3.m[4] << ' ' << mat3.m[5] << '\n'
                  << mat3.m[6] << ' ' << mat3.m[7] << ' ' << mat3.m[8];
    }
};
}  // namespace vulk
