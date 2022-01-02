option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(${PROJECT_PREFIX}_WITH_SCOPED_PROFILER "Enable scoped profiler" OFF)
option(${PROJECT_PREFIX}_ENABLE_IPO "Enable InterProcedural Optimizations [Release mode only]" ON)

if (${PROJECT_PREFIX}_WITH_SCOPED_PROFILER)
    add_compile_definitions(${PROJECT_PREFIX}_WITH_SCOPED_PROFILER=1)
else ()
    add_compile_definitions(${PROJECT_PREFIX}_WITH_SCOPED_PROFILER=0)
endif ()
