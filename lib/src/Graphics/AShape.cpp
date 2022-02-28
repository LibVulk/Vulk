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

#include "Vulk/Graphics/AShape.hpp"

#include "Vulk/Contexts/ContextVulkan.hpp"

vulk::AShape::AShape()
{
    detail::ContextVulkan::getInstance().registerDrawable(*this);
}

vulk::AShape::~AShape()
{
    resetBuffers();
    detail::ContextVulkan::getInstance().unregisterDrawable(*this);
}

void vulk::AShape::makeBuffers()
{
    auto& vulkInstance = detail::ContextVulkan::getInstance();

    vulkInstance.createBuffers(m_vertices, m_vertexBuffer, m_vertexMemory, vk::BufferUsageFlagBits::eVertexBuffer);
    vulkInstance.createBuffers(m_indices, m_indexBuffer, m_indexMemory, vk::BufferUsageFlagBits::eIndexBuffer);
}

void vulk::AShape::resetBuffers()
{
    auto& vulkInstance = detail::ContextVulkan::getInstance();

    vulkInstance.destroyBuffer(m_indexBuffer);
    vulkInstance.destroyBuffer(m_vertexBuffer);
    vulkInstance.freeBufferMem(m_indexMemory);
    vulkInstance.freeBufferMem(m_vertexMemory);
}

void vulk::AShape::setColor(vulk::Color color)
{
    resetBuffers();
    for (auto& vertex : m_vertices)
    {
        vertex.color = color.to_glm_color();
    }
    makeBuffers();
}
