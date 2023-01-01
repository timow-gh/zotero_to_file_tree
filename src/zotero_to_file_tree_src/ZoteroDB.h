#ifndef ZOTERO_TO_FILE_TREE_ZOTERODB_H
#define ZOTERO_TO_FILE_TREE_ZOTERODB_H

#include "PDFItem.h"
#include "ZoteroCollection.h"
#include <cstdint>
#include <filesystem>
#include <set>
#include <unordered_map>

namespace zotfiles
{

struct ZoteroDBInfo
{
  // schema = version;
  std::uint32_t userdata{};
  std::uint32_t triggers{};
  std::uint32_t translators{};
  std::uint32_t system{};
  std::uint32_t styles{};
  std::uint32_t repository{};
  std::uint32_t globalSchema{};
  std::uint32_t deletes{}; // original: delete, but delete is a keyword
  std::uint32_t compatibility{};
};

[[nodiscard]] ZoteroDBInfo supported_zotero_db_info();
[[nodiscard]] std::string print_table(const ZoteroDBInfo& info);

[[nodiscard]] std::string_view zotero_db_name();
[[nodiscard]] std::filesystem::path create_zotero_db_path(const std::string& library_path_str);
[[nodiscard]] ZoteroDBInfo db_info(const std::filesystem::path& zotero_db_path);
[[nodiscard]] bool is_supported_zotero_db(const std::filesystem::path& zotero_db_path);

// Retrieves all pdf attachments from the zotero db.
[[nodiscard]] std::vector<PDFAttachment> pdf_attachments(const std::filesystem::path& zotero_db_path);

// All collections that are parents of the given collectionIDs that are not already in the given collections.
[[nodiscard]] std::set<ZoteroCollection> parent_collections(const std::set<std::int64_t>& collectionIds,
                                                            const std::filesystem::path& zotero_db_path);

[[nodiscard]] std::vector<PDFItem> pdf_items(const std::vector<zotfiles::PDFAttachment>& pdfAttachments,
                                             const std::filesystem::path& zoteroDbPath);
void retrieve_pdf_item_collections(std::vector<PDFItem>& pdfItems, const std::filesystem::path& zotero_db_path);
// Collects all pdf item collections and their parent collections
std::unordered_map<std::int64_t, ZoteroCollection> all_pdf_item_collections(const std::vector<PDFItem>& pdfItems,
                                                                            const std::filesystem::path& zoteroDBPath);

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTERODB_H
