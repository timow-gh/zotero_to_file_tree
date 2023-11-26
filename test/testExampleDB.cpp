#include "TestResources.h"
#include <ErrorCodes.hpp>
#include <ZoteroToFileTree.h>
#include <gtest/gtest.h>

TEST(ZoteroToFileTree, test_argument_help) {
  char* argv[] = {(char*)"zotero_to_file_tree", (char*)"--help"};
  int argc = 2;
  std::error_code errorCode = zotfiles::ZoteroToFileTree::run(argc, argv);
  std::error_code expected = zotfiles::make_error_code(zotfiles::ErrorCodes::SUCCESS);
  EXPECT_FALSE(errorCode);
  EXPECT_EQ(errorCode, expected);
}

TEST(ZoteroToFileTree, library_path_empty) {
  char* argv[] = {(char*)"zotero_to_file_tree", (char*)"-l"};
  int argc = 2;
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argc, argv), zotfiles::ErrorCodes::CLI_PARSE_ERROR);
}

TEST(ZoteroToFileTree, library_path_valid) {
  std::filesystem::path zoterDBPath = zotero_example_db();
  auto dbPathStr = zoterDBPath.string();
  char* argv[] = {(char*)"zotero_to_file_tree", (char*)"-l", (char*)dbPathStr.c_str()};
  int argc = 3;
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argc, argv), zotfiles::ErrorCodes::OUTPUT_DIR_INVALID);
}

TEST(ZoteroToFileTree, library_does_not_exist) {
  std::filesystem::path zoterDBPath = zotero_example_db();
  std::string zoterDBPathString = zoterDBPath.parent_path().string();
  char* argv[] = {(char*)"zotero_to_file_tree", (char*)"-l", (char*)zoterDBPathString.c_str()};
  int argc = 3;
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argc, argv), zotfiles::ErrorCodes::ZOTERO_DB_DOES_NOT_EXIST);
}

TEST(ZoteroToFileTree, library_not_supported) {
  std::filesystem::path zoterDBPath = zotero_example_db_depr();
  std::string zoterDBPathString = zoterDBPath.string();
  char* argv[] = {(char*)"zotero_to_file_tree", (char*)"-l", (char*)zoterDBPathString.c_str()};
  int argc = 3;
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argc, argv), zotfiles::ErrorCodes::ZOTERO_DB_NOT_SUPPORTED);
}
