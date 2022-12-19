cmake_minimum_required(VERSION 3.21) # CMakePresets are supported since version 3.21

project(zotero_to_file_tree
        VERSION 0.1.0
        LANGUAGES CXX)

# Make all modules available to this project and other projects that consume this project by
# adding the cmake directory to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(project_preamble)
include(setup_export_set_installation)

include(project_preamble)
project_preamble()

include(cmake/CPM.cmake)
set(CPM_SOURCE_CACHE "C:/Users/testUser/dev/repos/cpm_source_cache")
if (NOT CPM_SOURCE_CACHE)
    message(AUTHOR_WARNING "CPM_SOURCE_CACHE not set. The source files are downloaded for each project.")
endif ()

# Create options for the project
set(ENABLE_DEVELOPER_MODE
        TRUE
        CACHE BOOL "Enable 'developer mode'")

option(USE_SCCACHE "Use sccache" ON)
if (ENABLE_DEVELOPER_MODE)
    option(${PROJECT_NAME}_TESTS "Build tests" ON)
    option(${PROJECT_NAME}_BENCHMARKS "Build benchmarks" ON)
    option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
    option(${PROJECT_NAME}_STATIC_ANALYSIS "" ON)
else ()
    option(${PROJECT_NAME}_TESTS "Build tests" OFF)
    option(${PROJECT_NAME}_BENCHMARKS "Build benchmarks" OFF)
    option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
    option(${PROJECT_NAME}_STATIC_ANALYSIS "" OFF)
endif ()

option(${PROJECT_NAME}_PACKAGE "Use CPack to create an installer" ON)

if (USE_SCCACHE)
    if (NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja")
        message(FATAL_ERROR "sccache is only testet with Ninja.")
    endif ()

    find_program(CCACHE_PROGRAM sccache)
    message(STATUS "CCACHE_PROGRAM=${CCACHE_PROGRAM}")
    if (CCACHE_PROGRAM)
        # Makefile Generators and the Ninja generator prefix compiler commands with the given launcher command line.
        # This is intended to allow launchers to intercept build problems with high granularity. Other generators ignore
        # this property because their underlying build systems provide no hook to wrap individual commands with a launcher.
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else ()
        message(FATAL_ERROR "sccache not found. Please install it or disable USE_SCCACHE")
    endif ()

    include(cmake/sccache_setup.cmake)
    fix_msvc_ninja_compile_flags()
endif ()

add_subdirectory(dependencies)

add_subdirectory(src)

if (${PROJECT_NAME}_PACKAGE)
    add_subdirectory(packaging)
endif ()

#if (${PROJECT_NAME}_BENCHMARKS)
#    add_subdirectory(benchmark)
#endif ()

#if (${PROJECT_NAME}_TESTS)
#    include(CTest)
#    enable_testing()
#    add_subdirectory(test)
#endif ()

