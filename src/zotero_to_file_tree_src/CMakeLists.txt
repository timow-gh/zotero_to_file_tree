set(LIB_NAME zotero_to_file_tree_lib)

add_library(${LIB_NAME}
        ZoteroToFileTree.cpp
        ZoteroDB.cpp
        CollectionTree.cpp
        CollectionTree.hpp
        ZoteroCollection.hpp
        PDFItem.hpp
        ZoteroPDFAttachment.hpp
        ErrorCodes.hpp
        ErrorCodes.cpp
)
target_link_libraries(${LIB_NAME} PRIVATE fmt::fmt SQLiteCpp PUBLIC CLI11::CLI11)
add_library(${LIB_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

set_target_properties(${LIB_NAME}
        PROPERTIES
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
        DEBUG_POSTFIX d)

include(warnings)
add_warnings_and_compile_options(${LIB_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})

if (${PROJECT_NAME}_STATIC_ANALYSIS)
    include(static_analysis)
    enable_static_analysis(${LIB_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})
endif ()

set(EXE_NAME zotero_to_file_tree)
add_executable(${EXE_NAME} ZoteroToFileTreeCli.cpp)
target_link_libraries(${EXE_NAME} PRIVATE zotero_to_file_tree_lib::zotero_to_file_tree_lib)

set_target_properties(${EXE_NAME}
        PROPERTIES
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
        DEBUG_POSTFIX d
        EXPORT_NAME zotero_to_file_tree)

include(warnings)
add_warnings_and_compile_options(${EXE_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})

if (${PROJECT_NAME}_STATIC_ANALYSIS)
    include(static_analysis)
    enable_static_analysis(${EXE_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})
endif ()
