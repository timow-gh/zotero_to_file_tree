#ifndef ZOTERO_TO_FILE_TREE_ZOTERODB_H
#define ZOTERO_TO_FILE_TREE_ZOTERODB_H

#include <cstdint>
#include <filesystem>
#include <ostream>

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

std::string print_table(const ZoteroDBInfo& info);

[[nodiscard]] ZoteroDBInfo supported_zotero_db_info();
[[nodiscard]] ZoteroDBInfo zotero_db_info(const std::filesystem::path& zotero_db_path);
[[nodiscard]] bool is_supported_zotero_db(const std::filesystem::path& zotero_db_path);

struct PdfItem
{
  std::int64_t itemID{};
  std::string path{}; // member is named after path in the zotero db. This is not a real path, but the name of the pdf file stored.
  std::string collectionName{};
  std::int64_t collectionID{};
  std::int64_t parentCollectionID{};
  std::string key{};
};

struct PdfItemWithPath
{
  zotfiles::PdfItem pdfItem{};
  std::filesystem::path pdfFilePath{};
};

[[nodiscard]] std::vector<PdfItemWithPath> pdf_items(const std::filesystem::path& zotero_db_path);

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTERODB_H
