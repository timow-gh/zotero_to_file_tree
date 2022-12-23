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
 * - add a command line option to specify the output directory
 * - add a command line option to specify the collection name (optional)
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment in a specific collection
 * - add sql statement to get all std::filesystem paths of items with a pdf attachment in a specific collection and its subcollections
 * - Build a tree of the collection hierarchy, each node has the name of the collection and a list of std::filesystem paths to the pdfs
 *  - allow or disallow duplicates paths of pdfs in the tree
 * - add option to query only files of a specific user name
 * - add option to query only files of a specific library
 */

static constexpr std::string_view zotero_db_name = "zotero.sqlite";

} // namespace zotfiles

int main(int argc, char** argv)
{
  CLI::App app{"Converts a exported zotero library to a file tree."};

  std::string library_path_str{""};
  app.add_option("--zotero_dir", library_path_str, "Path to the zotero library. Default is the current directory.");

  CLI11_PARSE(app, argc, argv);

  std::filesystem::path zotero_lib_path = std::filesystem::path(library_path_str);
  if (!std::filesystem::exists(zotero_lib_path))
  {
    zotero_lib_path = std::filesystem::current_path();
  }

  if (std::filesystem::is_directory(zotero_lib_path))
  {
    zotero_lib_path /= zotfiles::zotero_db_name;
  }

  if (!std::filesystem::exists(zotero_lib_path) || !std::filesystem::is_regular_file(zotero_lib_path))
  {
    fmt::print("The zotero library path is invalid: {}\n", zotero_lib_path.string());
    return EXIT_FAILURE;
  }

  if (!zotfiles::is_supported_zotero_db(zotero_lib_path))
  {
    return EXIT_FAILURE;
  }

  zotfiles::ZoteroDBInfo zoteroDBInfo = zotfiles::zotero_db_info(zotero_lib_path);
  std::stringstream ss;
  ss << zoteroDBInfo;
  std::cout << ss.str();

  return 0;
}
