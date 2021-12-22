# Conan CMake setup

macro(run_conan)
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan/conan.cmake")
    endif ()

    include(${CMAKE_BINARY_DIR}/conan/conan.cmake)

    conan_add_remote(NAME bincrafters URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan)

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(_GLIBCXX_USE_CXX11_ABI)
    endif ()

    conan_cmake_run(
            # options
            BASIC_SETUP
            NO_OUTPUT_DIRS
            CMAKE_TARGETS
            TARGETS
            # oneValueArgs
            INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
            CONANFILE conanfile.txt
            # multiValueArgs
            BUILD missing
            INSTALL_ARGS
            REQUIRES ${CONAN_EXTRA_REQUIRES}
            OPTIONS ${CONAN_EXTRA_OPTIONS}
    )
endmacro()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/conan")
