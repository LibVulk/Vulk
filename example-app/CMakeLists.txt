add_executable(
        ${PROJECT_NAME}-tests
        include/Main.hpp src/Main.cpp
)
target_link_libraries(${PROJECT_NAME}-tests PUBLIC ${PROJECT_NAME} glfw vulkan)
target_include_directories(${PROJECT_NAME}-tests PRIVATE include/ ../lib/include)
