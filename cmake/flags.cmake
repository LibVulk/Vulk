if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Enable if using std::execution
    #    link_libraries(tbb)
    #    add_compile_definitions(TBB_SUPPRESS_DEPRECATED_MESSAGES=1) # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101228
endif ()

if (MSVC)
    add_link_options(/ignore:4099)
    #    link_libraries(legacy_stdio_definitions)
endif ()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(${PROJECT_PREFIX}_DEBUG=1)
    add_compile_definitions(${PROJECT_PREFIX}_RELEASE=0)
else ()
    add_compile_definitions(${PROJECT_PREFIX}_DEBUG=0)
    add_compile_definitions(${PROJECT_PREFIX}_RELEASE=1)
endif ()

add_compile_definitions(GLFW_INCLUDE_VULKAN=1)
add_compile_definitions(GLM_FORCE_RADIANS=1)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE=1)
