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

#include <Vulk/Contexts/ContextVulkan.hpp>
#include <Vulk/Window.hpp>

#include <iostream>

int main()
{
    vulk::Window win{800, 600, "Vulkan window"};

    win.getFrameManager().setOnSecondCallback([&win](const vulk::FrameManager& frameManager) {
        constexpr const char* TitleFormat = "Vulkan window (%u fps)";
        constexpr size_t BufferSize = 32;

        char buffer[BufferSize]{};

        std::snprintf(buffer, BufferSize, TitleFormat, frameManager.getFPS());

        win.setTitle(buffer);
    });

    while (win.isOpen())
    {
        win.pollEvents();

        win.display();
    }

    return 0;
}
