#ifndef ZOTERO_TO_FILE_TREE_SQLSTATEMENTS_H
#define ZOTERO_TO_FILE_TREE_SQLSTATEMENTS_H

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

  friend std::stringstream& operator<<(std::stringstream& stringstream, const ZoteroDBInfo& info)
  {
    stringstream << "userdata: " << info.userdata << "\n"
                 << "triggers: " << info.triggers << "\n"
                 << "translators: " << info.translators << "\n"
                 << "system: " << info.system << "\n"
                 << "styles: " << info.styles << "\n"
                 << "repository: " << info.repository << "\n"
                 << "globalSchema: " << info.globalSchema << "\n"
                 << "delete: " << info.deletes << "\n"
                 << "compatibility: " << info.compatibility << "\n";
    return stringstream;
  }
};

[[nodiscard]] ZoteroDBInfo supported_zotero_db_info();
[[nodiscard]] ZoteroDBInfo zotero_db_info(const std::filesystem::path& zotero_db_path);
[[nodiscard]] bool is_supported_zotero_db(const std::filesystem::path& zotero_db_path);

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_SQLSTATEMENTS_H
