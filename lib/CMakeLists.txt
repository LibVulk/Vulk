add_library(
        ${PROJECT_NAME} STATIC
        include/SFVL/Time.hpp
        src/Window.cpp include/SFVL/Window.hpp
        src/Contexts/ContextGLFW.cpp include/SFVL/Contexts/ContextGLFW.hpp
        src/Error.cpp include/SFVL/Error.hpp
        src/Contexts/ContextVulkan.cpp include/SFVL/Contexts/ContextVulkan.hpp
        src/FrameManager.cpp include/SFVL/FrameManager.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include/SFVL)
