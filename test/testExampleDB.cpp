#include "TestResources.h"
#include <gtest/gtest.h>

#include <ErrorCodes.hpp>

#include <ZoteroToFileTree.hpp>


class ZoteroFileTreeTest : public testing::Test
{
  protected:
    static char executableName[];
    static char argHelp[];
    static char shortArgLibrary[];
    static char argLibrary[];
};

char ZoteroFileTreeTest::executableName[] = "zotero_to_file_tree";
char ZoteroFileTreeTest::argHelp[] = "--help";
char ZoteroFileTreeTest::shortArgLibrary[] = "-l";
char ZoteroFileTreeTest::argLibrary[] = "--lib";

TEST_F(ZoteroFileTreeTest, test_argument_help) {
  std::array<char*, 2> argv = { executableName, argHelp };
  std::error_code errorCode = zotfiles::ZoteroToFileTree::run(2, argv.data());
  std::error_code expected = zotfiles::make_error_code(zotfiles::ErrorCodes::SUCCESS);
  EXPECT_FALSE(errorCode);
  EXPECT_EQ(errorCode, expected);
}

TEST_F(ZoteroFileTreeTest, library_path_empty) {
  std::array<char*, 2> argv = { executableName, shortArgLibrary };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::CLI_PARSE_ERROR);

  argv = { executableName, argLibrary };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::CLI_PARSE_ERROR);
}

TEST_F(ZoteroFileTreeTest, library_path_valid) {
  std::filesystem::path zoterDBPath = zotero_example_db();
  const auto dbPathStr = zoterDBPath.string();
  char* dbPathArg = (char*)dbPathStr.c_str();

  std::array<char*, 3> argv = { executableName, shortArgLibrary, dbPathArg};
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::OUTPUT_DIR_INVALID);

  argv = { executableName, argLibrary, dbPathArg};
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::OUTPUT_DIR_INVALID);
}

TEST_F(ZoteroFileTreeTest, library_does_not_exist) {
  std::filesystem::path zoterDBPath = zotero_example_db();
  std::string zoteroDBPathString = zoterDBPath.parent_path().string();
  char* dbPathArg = (char*)zoteroDBPathString.c_str();

  std::array<char*, 3> argv = { executableName, shortArgLibrary, dbPathArg };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::ZOTERO_DB_DOES_NOT_EXIST);

  argv = { executableName, argLibrary, dbPathArg };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::ZOTERO_DB_DOES_NOT_EXIST);
}

TEST_F(ZoteroFileTreeTest, library_not_supported) {
  std::filesystem::path zoterDBPath = zotero_example_db_depr();
  std::string zoteroDBPathString = zoterDBPath.string();
  char* dbPathArg = (char*)zoteroDBPathString.c_str();

  std::array<char*, 3> argv = { executableName, shortArgLibrary, dbPathArg };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::ZOTERO_DB_NOT_SUPPORTED);

  argv = { executableName, argLibrary, dbPathArg };
  EXPECT_EQ(zotfiles::ZoteroToFileTree::run(argv.size(), argv.data()), zotfiles::ErrorCodes::ZOTERO_DB_NOT_SUPPORTED);
}
