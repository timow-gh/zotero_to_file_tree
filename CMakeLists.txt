cmake_minimum_required(VERSION 3.21) # CMakePresets are supported since version 3.21

project(zotero_to_file_tree
        VERSION 0.1.0
        LANGUAGES CXX)

# Make all modules available to this project and other projects that consume this project by
# adding the cmake directory to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(project_preamble)
include(setup_export_set_installation)

project_preamble()

include(cmake/CPM.cmake)

# Create options for the project
set(ENABLE_DEVELOPER_MODE
        TRUE
        CACHE BOOL "Enable 'developer mode'")

if ((CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR))
    set(IS_TOP_LEVEL_PROJECT TRUE)
else ()
    set(IS_TOP_LEVEL_PROJECT FALSE)
endif ()

if (IS_TOP_LEVEL_PROJECT)
    option(${PROJECT_NAME}_USE_SCCACHE "Use sccache" ON)
    option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
    option(${PROJECT_NAME}_STATIC_ANALYSIS "" ON)
    option(${PROJECT_NAME}_TESTS "" ON)

    # check if the file CPMSourceVariable.cmake exists in project root
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CPMSourceVariable.cmake")
        include(CPMSourceVariable.cmake)
    else ()
        message(AUTHOR_WARNING "CPMSourceVariable.cmake not found. Set the CPM_SOURCE_CACHE variable to a directory to cache the git clones or create a the CPMSourceVariable.cmake file in the project root. Set the CPM_SOURCE_CACHE variable in CPMSourceVariable.cmake to keep your paths private.")
    endif ()

    if (NOT CPM_SOURCE_CACHE)
        message(AUTHOR_WARNING "CPM_SOURCE_CACHE not set. The source files are downloaded for each project.")
    endif ()
else ()
    option(${PROJECT_NAME}_USE_SCCACHE "Use sccache" OFF)
    option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
    option(${PROJECT_NAME}_STATIC_ANALYSIS "" OFF)
    option(${PROJECT_NAME}_TESTS "" OFF)
endif ()

if (${PROJECT_NAME}_USE_SCCACHE)
    if (NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja")
        message(AUTHOR_WARNING "sccache is only testet with Ninja. Setting ${PROJECT_NAME}_USE_SCCACHE to OFF.")
        set(${PROJECT_NAME}_USE_SCCACHE OFF CACHE BOOL "" FORCE)
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
        message(FATAL_ERROR "sccache not found. Please install it or disable ${PROJECT_NAME}_USE_SCCACHE")
    endif ()

    include(cmake/sccache_setup.cmake)
    fix_msvc_ninja_compile_flags()
endif ()

message(STATUS "${PROJECT_NAME}_USE_SCCACHE: ${${PROJECT_NAME}_USE_SCCACHE}")
message(STATUS "${PROJECT_NAME}_WARNINGS_AS_ERRORS: ${${PROJECT_NAME}_WARNINGS_AS_ERRORS}")
message(STATUS "${PROJECT_NAME}_STATIC_ANALYSIS: ${${PROJECT_NAME}_STATIC_ANALYSIS}")

find_program(DOXYGEN_EXECUTABLE doxygen)

if (NOT DOXYGEN_EXECUTABLE)
    message(AUTHOR_WARNING "Doxygen not found. Documentation will not be generated.")
endif ()

# if doxygen is found, generate the documentation
if (DOXYGEN_EXECUTABLE)
    message(STATUS "Doxygen found. Documentation will be generated.")
    message(STATUS "DOXGEN_PROGRAM=${DOXGEN_PROGRAM}")

    # configure doxygen file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile @ONLY)

    # Define the custom target for building the documentation
    add_custom_target(
            docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
    )
endif ()

add_subdirectory(dependencies)

add_subdirectory(src)

if (${PROJECT_NAME}_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif ()