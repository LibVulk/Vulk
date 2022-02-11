add_library(
        ${PROJECT_NAME} STATIC
        include/Vulk/Time.hpp
        include/Vulk/Vec2.hpp
        include/Vulk/Vec3.hpp
        include/Vulk/Rect.hpp
        include/Vulk/Mat3.hpp
        include/Vulk/ClassUtils.hpp
        src/Window.cpp include/Vulk/Window.hpp
        src/Exceptions.cpp include/Vulk/Exceptions.hpp
        src/Contexts/ContextGLFW.cpp include/Vulk/Contexts/ContextGLFW.hpp
        src/Contexts/ContextVulkan.cpp include/Vulk/Contexts/ContextVulkan.hpp
        src/FrameManager.cpp include/Vulk/FrameManager.hpp
        src/ScopedProfiler.cpp include/Vulk/ScopedProfiler.hpp
        src/Shader.cpp include/Vulk/Shader.hpp
        src/Utils.cpp include/Vulk/Utils.hpp
        src/Keyboard.cpp include/Vulk/Keyboard.hpp
        src/Mouse.cpp include/Vulk/Mouse.hpp
        src/Vertex.cpp include/Vulk/Vertex.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

if (${PROJECT_PREFIX}_ENABLE_PCH)
    # Saves compile time, but the project **has** to build without them (it is checked in the CI)
    target_precompile_headers(
            ${PROJECT_NAME} PRIVATE
            <cstddef>
            <cstdint>
            <vulkan/vulkan.hpp>
    )
endif ()

add_shader(${PROJECT_NAME} shader.frag)
add_shader(${PROJECT_NAME} shader.vert)
