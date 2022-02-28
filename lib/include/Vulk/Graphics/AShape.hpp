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

#include <vulkan/vulkan.hpp>

#include "Vulk/Graphics/ADrawable.hpp"
#include "Vulk/Objects.hpp"

namespace vulk {
class AShape : public ADrawable
{
public:
    AShape();
    ~AShape() override;
    explicit AShape(const glm::vec2& position) : ADrawable{position} {}

    [[nodiscard]] size_t getVertexCount() const noexcept { return m_vertices.size(); }
    [[nodiscard]] size_t getIndexCount() const noexcept { return m_indices.size(); }

    // TODO: these two should NOT be public, dirty workaround until a proper renderer is made
    [[nodiscard]] const auto& getVertexBuffer() const noexcept { return m_vertexBuffer; }
    [[nodiscard]] const auto& getIndexBuffer() const noexcept { return m_indexBuffer; }

protected:
    static constexpr vk::IndexType INDEX_TYPE = vk::IndexType::eUint32;

    template<size_t Count>
    void setVertices(const std::array<vulk::Vertex, Count>& vertices)
    {
        m_vertices.reserve(Count);
        m_vertices.insert(m_vertices.end(), vertices.cbegin(), vertices.cend());
    }

    template<size_t Count>
    void setIndices(const std::array<uint32_t, Count>& indices)
    {
        m_indices.reserve(Count);
        m_indices.insert(m_indices.end(), indices.cbegin(), indices.cend());
    }

    void makeBuffers();

private:
    vk::Buffer m_vertexBuffer{};
    vk::DeviceMemory m_vertexMemory{};
    vk::Buffer m_indexBuffer{};
    vk::DeviceMemory m_indexMemory{};

    std::vector<Vertex> m_vertices{};
    std::vector<uint32_t> m_indices{};
};
}  // namespace vulk
