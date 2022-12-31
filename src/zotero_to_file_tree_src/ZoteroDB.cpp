#include "ZoteroDB.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <fmt/format.h>
#include <functional>
#include <unordered_map>

namespace zotfiles
{

ZoteroDBInfo supported_zotero_db_info()
{
  static constexpr ZoteroDBInfo supported_zotero_db_info{
      120,        // userdata
      18,         // triggers
      1668085253, // translators
      32,         // system
      1668085253, // styles
      1671627457, // repository
      21,         // globalSchema
      74,         // delete
      7           // compatibility;
  };
  return supported_zotero_db_info;
}
std::string print_table(const ZoteroDBInfo& info)
{
  std::string table;
  table += fmt::format("ZoteroDBInfo:\n");
  table += fmt::format("{:-<29}\n", "");
  table += fmt::format("|{:<15}|{:>11}|\n", "userdata", info.userdata);
  table += fmt::format("|{:<15}|{:>11}|\n", "triggers", info.triggers);
  table += fmt::format("|{:<15}|{:>11}|\n", "translators", info.translators);
  table += fmt::format("|{:<15}|{:>11}|\n", "system", info.system);
  table += fmt::format("|{:<15}|{:>11}|\n", "styles", info.styles);
  table += fmt::format("|{:<15}|{:>11}|\n", "repository", info.repository);
  table += fmt::format("|{:<15}|{:>11}|\n", "globalSchema", info.globalSchema);
  table += fmt::format("|{:<15}|{:>11}|\n", "deletes", info.deletes);
  table += fmt::format("|{:<15}|{:>11}|\n", "compatibility", info.compatibility);
  table += fmt::format("{:-<29}\n", "");
  return table;
}
static void insertDBValue(ZoteroDBInfo& info, const std::string_view key, std::int32_t val)
{
  static auto zoterDBInfoSetter = std::unordered_map<std::string_view, std::function<void(ZoteroDBInfo & info, uint32_t value)>>{
      {"userdata", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.userdata = value; }},
      {"triggers", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.triggers = value; }},
      {"translators", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.translators = value; }},
      {"system", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.system = value; }},
      {"styles", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.styles = value; }},
      {"repository", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.repository = value; }},
      {"globalSchema", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.globalSchema = value; }},
      {"delete", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.deletes = value; }},
      {"compatibility", [](ZoteroDBInfo& zInfo, std::uint32_t value) { zInfo.compatibility = value; }},
  };
  auto it = zoterDBInfoSetter.find(key);
  if (it != zoterDBInfoSetter.end())
  {
    it->second(info, static_cast<std::uint32_t>(val));
  }
}

ZoteroDBInfo zotero_db_info(const std::filesystem::path& zotero_db_path)
{
  assert(std::filesystem::exists(zotero_db_path));

  ZoteroDBInfo zotero_db_info;
  try
  {
    // Open a database file in create/write mode
    SQLite::Database db(zotero_db_path, SQLite::OPEN_READONLY);

    auto query = SQLite::Statement(db, fmt::format("SELECT * FROM version"));
    while (query.executeStep())
    {
      // Get the collection name
      if (!query.isColumnNull(0))
      {
        insertDBValue(zotero_db_info, query.getColumn(0).getText(), query.getColumn(1).getInt());
      }
    }
  }
  catch (std::exception& e)
  {
    fmt::print("SQLite exception: {}\n", std::string(e.what()));
    std::abort();
  }

  return zotero_db_info;
}
bool is_supported_zotero_db(const std::filesystem::path& zotero_db_path)
{
  auto zoteroDBInfo = zotero_db_info(zotero_db_path);
  auto supportedZoteroDBInfo = supported_zotero_db_info();

  if (zoteroDBInfo.userdata != supportedZoteroDBInfo.userdata)
  {
    fmt::print("The zotero library is not supported: userdata version mismatch.\n");
    return false;
  }

  if (zoteroDBInfo.globalSchema != supportedZoteroDBInfo.globalSchema)
  {
    fmt::print("The zotero library is not supported: globalSchema version mismatch.\n");
    return false;
  }

  return true;
}
std::vector<PdfItemWithPath> pdf_items(const std::filesystem::path& zotero_db_path)
{
  std::vector<PdfItemWithPath> pdf_items;
  try
  {
    const SQLite::Database db(zotero_db_path, SQLite::OPEN_READONLY);
    SQLite::Statement query(db,
                            "SELECT \n"
                            "itemAttachments.itemID,\n"
                            "itemAttachments.path,\n"
                            "collections.collectionName,\n"
                            "collections.collectionID,\n"
                            "collections.parentCollectionID,\n"
                            "items.key\n"
                            "FROM \n"
                            "itemAttachments \n"
                            "LEFT JOIN collectionItems\n"
                            "ON collectionItems.itemID = itemAttachments.itemID\n"
                            "LEFT JOIN collections\n"
                            "ON collections.collectionID = collectionItems.collectionID\n"
                            "LEFT JOIN items\n"
                            "ON items.itemID = itemAttachments.itemID\n"
                            "WHERE itemAttachments.contentType = 'application/pdf'");
    while (query.executeStep())
    {
      pdf_items.push_back(PdfItemWithPath{query.getColumn(0).getInt(),
                                          query.getColumn(1).getText(),
                                          query.getColumn(2).getText(),
                                          query.getColumn(3).getInt(),
                                          query.getColumn(4).getInt(),
                                          query.getColumn(5).getText()});
    }
  }
  catch (std::exception& e)
  {
    fmt::print("SQLite exception: {}\n", std::string(e.what()));
    std::abort();
  }
  return pdf_items;
}
} // namespace zotfiles
