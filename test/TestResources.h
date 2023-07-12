#ifndef ZOTERO_TO_FILE_TREE_TESTRESOURCES_H
#define ZOTERO_TO_FILE_TREE_TESTRESOURCES_H

#include <filesystem>

/** @brief Returns the path to the test resources directory of a depr zotero db.
 */
std::filesystem::path zotero_example_db_depr();

/** @brief Returns the path to the test resources directory of a supported zotero db.
 */
std::filesystem::path zotero_example_db();

#endif // ZOTERO_TO_FILE_TREE_TESTRESOURCES_H
