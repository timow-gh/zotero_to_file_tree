CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG release-1.12.1
        GIT_SHALLOW TRUE
        OPTIONS "INSTALL_GTEST OFF"
)

if (googletest_ADDED)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(gtest_main INTERFACE
                "-Wno-weak-vtables"
        )
    endif ()
endif ()

function(create_cli_test testName)
    add_executable(${testName} ${testName}.cpp TestResources.cpp TestResources.h)

    # Resource path to the valid zotero_example_db
    target_compile_definitions(${testName} PRIVATE RESOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}/zotero_example_db)
    # Resource path to the deprecated zotero_example_db
    target_compile_definitions(${testName} PRIVATE RESOURCE_DIR_DEPR=${CMAKE_CURRENT_LIST_DIR}/zotero_example_db_depr)

    target_link_libraries(${testName} PRIVATE gtest_main zotero_to_file_tree_lib::zotero_to_file_tree_lib CLI11::CLI11)
    add_test(NAME zotero_to_file_tree.${testName} COMMAND ${testName})
endfunction()

create_cli_test(testExampleDB)
