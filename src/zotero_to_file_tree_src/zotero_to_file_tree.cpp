#include "CollectionTree.h"
#include "ZoteroDB.h"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <string_view>
#include <vector>

namespace zotfiles
{
/*
 * TODO:
 * - add a command line option to specify the collection name (optional)
 * - allow or disallow duplicates paths of pdfs in the tree
 * - add option to query only files of a specific user name
 * - add option to query only files of a specific library
 */

[[nodiscard]] std::vector<PDFItem> create_pdfitems(const std::filesystem::path& zoteroDbPath)
{
  const std::vector<zotfiles::PDFAttachment> pdfAttachments = zotfiles::pdf_attachments(zoteroDbPath);
  std::vector<zotfiles::PDFItem> pdfItems = zotfiles::pdf_items(pdfAttachments, zoteroDbPath);
  auto removeEndIter = std::remove_if(pdfItems.begin(),
                                      pdfItems.end(),
                                      [](const zotfiles::PDFItem& item) { return !std::filesystem::exists(item.pdfFilePath); });
  pdfItems.erase(removeEndIter, pdfItems.end());
  zotfiles::retrieve_pdf_item_collections(pdfItems, zoteroDbPath);
  return pdfItems;
}

[[nodiscard]] CollectionTree create_collectiontree(const std::vector<zotfiles::PDFItem>& pdfItems,
                                                   const std::filesystem::path& zoteroDbPath)
{
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

[[nodiscard]] std::filesystem::path create_output_dir(const std::string& outputDirStr, bool overwriteOutputDir)
{
  const std::filesystem::path outputDirPath = std::filesystem::path(outputDirStr);

  bool outputDirExists = std::filesystem::exists(outputDirPath);
  if (outputDirExists && overwriteOutputDir)
  {
    fmt::print("The output directory already exists and will be overwritten: {}\n", outputDirPath.string());
    std::filesystem::remove_all(outputDirPath);
  }

  if (!outputDirExists)
  {
    std::filesystem::create_directories(outputDirPath);
  }
  return outputDirPath;
}

} // namespace zotfiles

int main(int argc, char** argv)
{
  std::locale::global(std::locale("en_US.UTF-8"));

  CLI::App app{"Converts a exported zotero library to a file tree."};

  std::string libraryPathStr;
  app.add_option("-l,--lib", libraryPathStr, "Path to the zotero library. Default is the current directory.");

  std::string outputDirStr;
  app.add_option("-o,--output_dir", outputDirStr, "Path to the output directory.")->required();

  bool printZoteroDBInfo{false};
  app.add_flag("--print_db_info", printZoteroDBInfo, "Print the zotero db info.");

  bool overwriteAll{false};
  app.add_flag("--overwrite_all", overwriteAll, "Overwrite the output directory if it exists.");

  bool skipExistingFiles{false};
  app.add_flag("--skip_existing", skipExistingFiles, "Skip existing files if they exist in the output directory.");

  CLI11_PARSE(app, argc, argv);

  auto zoteroDbPath = zotfiles::create_zotero_db_path(libraryPathStr);
  if (std::filesystem::exists(zoteroDbPath))
  {
    fmt::print("Zotero db path: {}\n", zoteroDbPath.string());
  }
  else
  {
    fmt::print("The zotero library path does not exist:\n {}\n", zoteroDbPath.string());
    return EXIT_FAILURE;
  }

  if (printZoteroDBInfo)
  {
    const zotfiles::ZoteroDBInfo zoteroDBInfo = zotfiles::db_info(zoteroDbPath);
    fmt::print("\n{}\n", zotfiles::print_table(zoteroDBInfo));
  }

  std::filesystem::path outputDirPath = zotfiles::create_output_dir(outputDirStr, overwriteAll);
  if (outputDirPath.empty())
  {
    fmt::print("The output directory path is not valid.\n");
    return EXIT_FAILURE;
  }

  const std::vector<zotfiles::PDFItem> pdfItems = zotfiles::create_pdfitems(zoteroDbPath);

  const auto numPdfItems = pdfItems.size();
  fmt::print("Number of pdf items with a valid pdf path: {}\n", numPdfItems);
  const auto numInvalidPdfItems = numPdfItems - pdfItems.size();
  fmt::print("Number of pdf items with an invalid pdf path: {}\n", numInvalidPdfItems);

  zotfiles::CollectionTree collectionTree = create_collectiontree(pdfItems, zoteroDbPath);

  std::size_t writtenPDFs = collectionTree.write_pdfs(outputDirPath, skipExistingFiles);

  fmt::print("Number of pdfs written: {}\n", writtenPDFs);

  return 0;
}
