#include "ZoteroToFileTree.h"
#include "CLI/Error.hpp"
#include "CollectionTree.h"
#include "ErrorCodes.hpp"
#include "ZoteroDB.h"
#include "fmt/core.h"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <string_view>
#include <system_error>
#include <vector>

namespace zotfiles
{

/*
 * TODO:
 * - add a command line option to specify the collection name (optional)
 * - allow or disallow duplicates paths of pdfs in the tree. Currently, PDFs that are in multiple collections, will be written in every
 * collection directory
 * - add option to query only files of a specific user name
 * - add option to query only files of a specific library
 */

[[nodiscard]] std::vector<PDFItem> ZoteroToFileTree::create_pdfitems(const std::filesystem::path& zoteroDbPath) {
  const std::vector<zotfiles::ZoteroPDFAttachment> pdfAttachments = zotfiles::pdf_attachments(zoteroDbPath);
  std::vector<zotfiles::PDFItem> pdfItems = zotfiles::pdf_items(pdfAttachments, zoteroDbPath);
  auto removeEndIter = std::remove_if(pdfItems.begin(),
                                      pdfItems.end(),
                                      [](const zotfiles::PDFItem& item) { return !std::filesystem::exists(item.pdfFilePath); });
  pdfItems.erase(removeEndIter, pdfItems.end());
  zotfiles::retrieve_pdf_item_collections(pdfItems, zoteroDbPath);
  return pdfItems;
}

[[nodiscard]] CollectionTree ZoteroToFileTree::create_collectiontree(const std::vector<zotfiles::PDFItem>& pdfItems,
                                                                     const std::filesystem::path& zoteroDbPath) {
  const std::unordered_map<std::int64_t, zotfiles::ZoteroCollection> pdfItemCollections =
      zotfiles::all_pdf_item_collections(pdfItems, zoteroDbPath);

  // Create the collection tree from the collectionItems
  std::unordered_map<std::int64_t, std::shared_ptr<zotfiles::CollectionNode>> collectionNodes;
  for (const auto& [collectionId, collection]: pdfItemCollections)
  {

    collectionNodes.emplace(
        collectionId,
        std::make_shared<zotfiles::CollectionNode>(
            zotfiles::CollectionNode{collection.collectionID, collection.parentCollectionID, collection.collectionName}));
  }

  zotfiles::CollectionTree collectionTree = zotfiles::CollectionTree::build(std::move(collectionNodes));

  std::for_each(pdfItems.begin(),
                pdfItems.end(),
                [&collectionTree](const zotfiles::PDFItem& pdfItem)
                {
                  for (const auto& collectionItem: pdfItem.collectionItems)
                  {
                    auto collection = collectionTree.find(collectionItem.collectionID);
                    if (collection)
                    {
                      auto iter = std::find_if(collection->collectionPDFItems.begin(),
                                               collection->collectionPDFItems.end(),
                                               [&pdfItem](const zotfiles::CollectionPDFItem& item)
                                               { return item.pdfName == pdfItem.pdfAttachment.path; });
                      if (iter == collection->collectionPDFItems.end())
                      {
                        collection->collectionPDFItems.emplace_back(
                            zotfiles::CollectionPDFItem{pdfItem.pdfAttachment.itemID, pdfItem.pdfAttachment.path, pdfItem.pdfFilePath});
                      }
                      else
                      {
                        fmt::print("Duplicate pdf item found: {} in collection: {}. Skipping.\n",
                                   pdfItem.pdfAttachment.path,
                                   collection->collectionName);
                      }
                    }
                  }
                });

  return collectionTree;
}

[[nodiscard]] std::filesystem::path ZoteroToFileTree::create_output_dir(const std::string& outputDirStr, bool overwriteOutputDir) {
  const std::filesystem::path outputDirPath = std::filesystem::path(outputDirStr);

  std::error_code errorCode;
  bool outputDirExists = std::filesystem::exists(outputDirPath, errorCode);
  if (errorCode)
  {
    fmt::print("Error while checking if the output directory exists: {}\n", errorCode.message());
    return {};
  }

  if (outputDirExists && overwriteOutputDir)
  {
    fmt::print("The output directory already exists and will be overwritten: {}\n", outputDirPath.string());
    errorCode.clear();
    std::filesystem::remove_all(outputDirPath, errorCode);
    if (errorCode)
    {
      fmt::print("Error while removing the output directory: {}\n", errorCode.message());
      return {};
    }
  }

  if (!outputDirExists)
  {
    errorCode.clear();
    if (!std::filesystem::create_directories(outputDirPath, errorCode))
    {
      fmt::print("Error while creating the output directory: {}\n", errorCode.message());
      return {};
    }
  }
  return outputDirPath;
}

/**
 *\brief Creates the path to the zotero db file.
 *
 * If the path given by library_path_str doesn't exist, the current directory is used to create the zotero db file path by appending the
 * standard zotero db file name.
 * If the path given by library_path_str is a directory, the standard zotero db file name is appended to the path and returned as the
 *  zotero db file path.
 *
 * @param library_path_str String containing the path to the zotero library or the directory with the zotero db file.
 * @return The std::filesystem::path to the zotero db file.
 */
[[nodiscard]] std::filesystem::path ZoteroToFileTree::create_zotero_db_path(const std::string& library_path_str) {
  std::filesystem::path zotero_lib_path = std::filesystem::path(library_path_str);

  if (!std::filesystem::exists(zotero_lib_path))
  {
    zotero_lib_path = std::filesystem::current_path() / standard_zotero_db_name();
  }

  if (std::filesystem::is_directory(zotero_lib_path))
  {
    zotero_lib_path /= standard_zotero_db_name();
  }

  return zotero_lib_path;
}

std::error_code ZoteroToFileTree::run(int argc, char** argv) {
  std::locale::global(std::locale("en_US.UTF-8"));

  CLI::App app{"Converts a exported zotero library to a file tree."};

  std::string libraryPathStr;
  app.add_option("-l,--lib", libraryPathStr, "Path to the zotero library. Default is the current directory.");

  std::string outputDirStr;
  app.add_option("-o,--output_dir", outputDirStr, "Path to the output directory.");

  bool printZoteroDBInfo{false};
  app.add_flag("--print_db_info", printZoteroDBInfo, "Print the zotero db info.");

  bool overwriteOutputDir{false};
  app.add_flag("--overwrite_dir",
               overwriteOutputDir,
               "Overwrite the output directory if it exists. All directories and files in the output directory will be deleted.");

  bool overwriteExistingFiles{false};
  app.add_flag("--overwrite_files", overwriteExistingFiles, "Skip existing files if they exist in the output directory.");

  try
  { app.parse((argc), (argv)); }
  catch (const CLI::ParseError& e)
  {
    app.exit(e);
    if (e.get_exit_code() == static_cast<int>(CLI::ExitCodes::Success))
    {
      return make_error_code(ErrorCodes::SUCCESS);
    }
    return make_error_code(ErrorCodes::CLI_PARSE_ERROR);
  }

  auto zoteroDbPath = create_zotero_db_path(libraryPathStr);
  if (std::filesystem::exists(zoteroDbPath))
  {
    fmt::print("Zotero db path: {}\n", zoteroDbPath.string());
  }
  else
  {
    fmt::print("The zotero library path does not exist:\n {}\n", zoteroDbPath.string());
    return make_error_code(ErrorCodes::ZOTERO_DB_DOES_NOT_EXIST);
  }

  if (printZoteroDBInfo)
  {
    const zotfiles::ZoteroDBInfo zoteroDBInfo = zotfiles::zotero_db_info(zoteroDbPath);
    fmt::print("Zotero db info:");
    fmt::print("\n{}\n", zotfiles::formatted_zotero_db_info(zoteroDBInfo));
    return make_error_code(zotfiles::ErrorCodes::SUCCESS);
  }

  if (!zotfiles::is_supported_zotero_db(zoteroDbPath))
  {
    return make_error_code(ErrorCodes::ZOTERO_DB_NOT_SUPPORTED);
  }

  std::filesystem::path outputDirPath = create_output_dir(outputDirStr, overwriteOutputDir);
  if (outputDirPath.empty())
  {
    fmt::print("The output directory path is not valid.\n");
    return make_error_code(ErrorCodes::OUTPUT_DIR_INVALID);
  }

  const std::vector<zotfiles::PDFItem> pdfItems = create_pdfitems(zoteroDbPath);

  const auto numPdfItems = pdfItems.size();
  fmt::print("Number of PDF items with a valid pdf path: {}\n", numPdfItems);
  const auto numInvalidPdfItems = numPdfItems - pdfItems.size();
  if (numInvalidPdfItems > 0)
  {
    fmt::print("Number of PDF items with an invalid pdf path: {}\n", numInvalidPdfItems);
  }

  fmt::print("\n");
  zotfiles::CollectionTree collectionTree = create_collectiontree(pdfItems, zoteroDbPath);
  auto [writtenPDFs, skippedPDFs] = collectionTree.write_pdfs(outputDirPath, overwriteExistingFiles);

  fmt::print("\nNumber of written PDFs: {}", writtenPDFs);
  if (skippedPDFs > 0)
  {
    fmt::print("\nNumber of existing PDFs skipped: {}", skippedPDFs);
  }

  return make_error_code(ErrorCodes::SUCCESS);
}
} // namespace zotfiles
