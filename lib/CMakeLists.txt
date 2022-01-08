add_library(
        ${PROJECT_NAME} STATIC
        include/SFVL/Time.hpp
        include/SFVL/Vec3.hpp
        src/Window.cpp include/SFVL/Window.hpp
        src/Contexts/ContextGLFW.cpp include/SFVL/Contexts/ContextGLFW.hpp
        src/Error.cpp include/SFVL/Error.hpp
        src/Contexts/ContextVulkan.cpp include/SFVL/Contexts/ContextVulkan.hpp
        src/FrameManager.cpp include/SFVL/FrameManager.hpp
        src/ScopedProfiler.cpp include/SFVL/ScopedProfiler.hpp
        src/Shader.cpp include/SFVL/Shader.hpp
        src/Utils.cpp include/SFVL/Utils.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include/SFVL)

if (${PROJECT_PREFIX}_ENABLE_PCH)
    # Saves compile time, but the project has to build without them (checked on the CI)
    target_precompile_headers(
            ${PROJECT_NAME} PRIVATE
            <cstddef>
            <cstdint>
            <vulkan/vulkan.hpp>
    )
endif ()

add_shader(${PROJECT_NAME} shader.frag)
add_shader(${PROJECT_NAME} shader.vert)
