#include "ZoteroDB.h"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <string_view>

namespace zotfiles
{
/*
 * TODO:
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

  bool printZoteroDBInfo{false};
  app.add_flag("--db_info", printZoteroDBInfo, "Print the zotero db info.");

  std::string outputDirStr{""};
  app.add_option("-o,--output_dir", outputDirStr, "Path to the output directory.")->required();

  bool overwriteOutputDir{false};
  app.add_flag("--overwrite", overwriteOutputDir, "Overwrite the output directory if it exists.");

  CLI11_PARSE(app, argc, argv);

  auto zoteroDbPath = zotfiles::get_zotero_db_path(libraryPathStr);
  if (std::filesystem::exists(zoteroDbPath))
  {
    fmt::print("Zotero db path: {}\n", zoteroDbPath.string());
  }
  else
  {
    fmt::print("The zotero library path is invalid: {}\n", zoteroDbPath.string());
    return EXIT_FAILURE;
  }

  if (printZoteroDBInfo)
  {
    const zotfiles::ZoteroDBInfo zoteroDBInfo = zotfiles::zotero_db_info(zoteroDbPath);
    fmt::print("\n{}\n", zotfiles::print_table(zoteroDBInfo));
  }

  auto outputDirPath = std::filesystem::path(outputDirStr);
  if (std::filesystem::exists(outputDirPath))
  {
    if (!overwriteOutputDir)
    {
      fmt::print("Aborting. The output directory already exists: {}\n", outputDirPath.string());
      return EXIT_FAILURE;
    }
    else
    {
      fmt::print("The output directory already exists and will be overwritten: {}\n", outputDirPath.string());
      std::filesystem::remove_all(outputDirPath);
      std::filesystem::create_directories(outputDirPath);
    }
  }

  if (!zotfiles::is_supported_zotero_db(zoteroDbPath))
  {
    return EXIT_FAILURE;
  }
  else
  {
    fmt::print("The zotero library is supported.\n");
  }

  auto pdfItems = zotfiles::pdf_items(zoteroDbPath);

  auto pdfItemsEnd =
      std::remove_if(pdfItems.begin(), pdfItems.end(), [](const zotfiles::PdfItemWithPath& item) { return item.pdfItem.path.empty(); });
  pdfItems.erase(pdfItemsEnd, pdfItems.end());

  const std::string_view pdfItemPathPrefix = "storage:";
  for (auto& item: pdfItems)
  {
    // check, if the path starts with the prefix pdfItemPathPrefix and remove it
    if (item.pdfItem.path.find(pdfItemPathPrefix) == 0)
    {
      item.pdfItem.path = item.pdfItem.path.substr(pdfItemPathPrefix.size());
    }

    // build path to the pdf file using the path to the zotero db directory. Use the subdirectory "storage", in this directory, there must
    // be a subdirectory with the name of the 'key' member of the pdf item.
    item.pdfFilePath = zoteroDbPath.parent_path() / "storage" / item.pdfItem.key / item.pdfItem.path;
  }

  // Use std::partition to separate the pdf items with a valid path from the ones with an invalid path
  auto pdfItemsEnd2 = std::partition(pdfItems.begin(),
                                     pdfItems.end(),
                                     [](const zotfiles::PdfItemWithPath& item) { return std::filesystem::exists(item.pdfFilePath); });

  // calc the number of pdf items with a valid path
  const auto numPdfItems = std::distance(pdfItems.begin(), pdfItemsEnd2);
  // calc the number of items without a valid path
  const auto numInvalidPdfItems = std::distance(pdfItemsEnd2, pdfItems.end());

  fmt::print("Number of pdf items: {}\n", pdfItems.size());
  fmt::print("Number of pdf items with a valid path: {}\n", numPdfItems);

  // remove the items without a valid path
  pdfItems.erase(pdfItemsEnd2, pdfItems.end());

  std::filesystem::create_directories(outputDirPath);

  // copy all pdf files to the output directory
  for (const auto& item: pdfItems)
  {
    std::filesystem::copy(item.pdfFilePath, outputDirPath / item.pdfItem.path);
  }

  return 0;
}
