cmake_minimum_required(VERSION 3.16)
project(vulk LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF) # turn on to debug Makefile builds

set(PROJECT_PREFIX VULK)

if (NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 20)
endif ()

if (NOT CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)

    message(WARNING "CMAKE_BUILD_TYPE undefined. Defaulting to Release.")
endif ()

include(cmake/conan.cmake)
run_conan()
link_directories(${CONAN_LIB_DIRS})
link_libraries(${CONAN_LIBS})
include_directories(SYSTEM "${CONAN_INCLUDE_DIRS}")

find_program(GLSLC glslc REQUIRED)

include(cmake/options.cmake)  # *needs to be first*
include(cmake/warnings.cmake)
include(cmake/flags.cmake)
include(cmake/functions.cmake)

if (${CMAKE_BUILD_TYPE} MATCHES "Release" AND ${${PROJECT_PREFIX}_ENABLE_IPO})
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_Supported OUTPUT ErrorMessage)

    if (IPO_Supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
        message(STATUS "Interprocedural optimizations enabled")
    endif ()
endif ()

add_subdirectory(lib)
add_subdirectory(example-app)
add_subdirectory(tests)
