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

#include <glm/vec2.hpp>

namespace vulk {
class ADrawable
{
public:
    ADrawable() = default;
    explicit ADrawable(const glm::vec2& position) : m_position{position} {}

    virtual ~ADrawable() = default;

    virtual void draw() const = 0;

    virtual void setPosition(const glm::vec2& position);

    [[nodiscard]] const auto& getPosition() const noexcept { return m_position; }

protected:
    glm::vec2 m_position{};
};
}  // namespace vulk
