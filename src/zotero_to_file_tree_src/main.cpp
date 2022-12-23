#include "SqlStatements.h"
#include "test_sql_statements.h"
#include <CLI/CLI.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>
#include <fmt/format.h>
#include <string>
#include <string_view>

namespace zotfiles
{
/*
 * TODO:
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment
 * - add a command line option to specify the collection name (optional)
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment in a specific collection
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment in a specific collection and its subcollections
 * - Build a tree of the collection hierarchy, each node has the name of the collection and a list of std::filesystem paths to the pdfs
 *  - allow or disallow duplicates paths of pdfs in the tree
 * - add option to query only files of a specific user name
 * - add option to query only files of a specific library
 */

static constexpr std::string_view zotero_db_name = "zotero.sqlite";

std::filesystem::path get_zotero_db_path(const std::string& library_path_str)
{
  std::filesystem::path zotero_lib_path = std::filesystem::path(library_path_str);
  if (!std::filesystem::exists(zotero_lib_path))
  {
    zotero_lib_path = std::filesystem::current_path();
  }

  if (std::filesystem::is_directory(zotero_lib_path))
  {
    zotero_lib_path /= zotfiles::zotero_db_name;
  }

  return zotero_lib_path;
}

} // namespace zotfiles

int main(int argc, char** argv)
{
  CLI::App app{"Converts a exported zotero library to a file tree."};

  std::string libraryPathStr{""};
  app.add_option("-l,--lib_path", libraryPathStr, "Path to the zotero library. Default is the current directory.");

  std::string outputDirStr{""};
  app.add_option("-o,--output_dir", outputDirStr, "Path to the output directory.")->required();

  CLI11_PARSE(app, argc, argv);

  auto zoteroDbPath = zotfiles::get_zotero_db_path(libraryPathStr);
  if (!std::filesystem::exists(zoteroDbPath))
  {
    fmt::print("The zotero library path is invalid: {}\n", zoteroDbPath.string());
    return EXIT_FAILURE;
  }

  auto outputDirPath = std::filesystem::path(outputDirStr);
  if (std::filesystem::exists(outputDirPath))
  {
    fmt::print("The output directory already exists: {}\n", outputDirPath.string());
    return EXIT_FAILURE;
  }

  // TODO: std::filesystem::create_directories(outputDirPath);
  // TODO: read items with pdf attachement form db

  if (!zotfiles::is_supported_zotero_db(zoteroDbPath))
  {
    return EXIT_FAILURE;
  }

  zotfiles::ZoteroDBInfo zoteroDBInfo = zotfiles::zotero_db_info(zoteroDbPath);
  std::stringstream ss;
  ss << zoteroDBInfo;
  std::cout << ss.str();

  return 0;
}
