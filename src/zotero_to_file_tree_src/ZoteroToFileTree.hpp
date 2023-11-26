#ifndef ZOTERO_TO_FILE_TREE_ZOTEROTOFILETREE_H
#define ZOTERO_TO_FILE_TREE_ZOTEROTOFILETREE_H

#include "CollectionTree.hpp"
#include "ErrorCodes.hpp"
#include "ZoteroDB.hpp"
#include <CLI/Error.hpp>
#include <filesystem>

namespace zotfiles
{

class ZoteroToFileTree {
public:
  static std::error_code run(int argc, char** argv);

private:
  [[nodiscard]] static std::vector<PDFItem> create_pdfitems(const std::filesystem::path& zoteroDbPath);
  [[nodiscard]] static CollectionTree create_collectiontree(const std::vector<zotfiles::PDFItem>& pdfItems,
                                                            const std::filesystem::path& zoteroDbPath);
  [[nodiscard]] static std::filesystem::path create_output_dir(const std::string& outputDirStr, bool overwriteOutputDir);
  [[nodiscard]] static std::filesystem::path create_zotero_db_path(const std::string& library_path_str);
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTEROTOFILETREE_H
