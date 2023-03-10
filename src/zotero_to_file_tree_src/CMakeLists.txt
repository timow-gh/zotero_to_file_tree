set(TARGET_NAME zotero_to_file_tree)

add_executable(${TARGET_NAME} zotero_to_file_tree.cpp ZoteroDB.cpp CollectionTree.cpp CollectionTree.h ZoteroCollection.h PDFItem.h ZoteroPDFAttachment.h)
target_link_libraries(${TARGET_NAME} PRIVATE CLI11::CLI11 fmt::fmt SQLiteCpp)

set_target_properties(${TARGET_NAME}
        PROPERTIES
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
        DEBUG_POSTFIX d
        VERSION ${${TARGET_NAME}_MAJOR}.${${TARGET_NAME}_MINOR}.${${TARGET_NAME}_PATCH}
        SOVERSION ${PROJECT_VERSION_MAJOR})

include(Warnings)
add_warnings_and_compile_options(${TARGET_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})

if (${PROJECT_NAME}_STATIC_ANALYSIS)
    include(static_analysis)
    enable_static_analysis(${TARGET_NAME} ${${PROJECT_NAME}_WARNINGS_AS_ERRORS})
endif ()

install(TARGETS
        ${TARGET_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})