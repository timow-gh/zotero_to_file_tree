#include "ZoteroDB.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <execution>
#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <unordered_map>

namespace zotfiles
{

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

// SQL querie to get the parent collections of the given pdf item ids
template <typename ForwardIter>
static std::string item_collections_query(ForwardIter itemIDBegin, ForwardIter itemIDEnd)
{
  static const std::string queryString = fmt::format(
      "SELECT \n"
      "items.itemID,\n"
      "collectionItems.collectionID,\n"
      "collections.parentCollectionID,\n"
      "collections.collectionName\n"
      "FROM \n"
      "items\n"
      "LEFT JOIN collectionItems\n"
      "ON collectionItems.itemID = items.itemID\n"
      "LEFT JOIN collections\n"
      "ON collections.collectionID = collectionItems.collectionID\n");

  return queryString + fmt::format("WHERE items.itemID IN ({})", fmt::join(itemIDBegin, itemIDEnd, ","));
}

class PDFItemIDsPolicy {
  std::vector<std::int64_t> pdfItemIDs;

public:
  using const_iterator = std::vector<std::int64_t>::const_iterator;

  template <typename Iter>
  void operator()(Iter begin, Iter end)
  {
    pdfItemIDs.reserve(std::distance(begin, end));
    collect_ids<PDFItem>(begin, end, std::back_inserter(pdfItemIDs));
  }

  [[nodiscard]] const_iterator cbegin() const { return pdfItemIDs.cbegin(); }
  [[nodiscard]] const_iterator cend() const { return pdfItemIDs.cend(); }
};

class PDFParentItemIdsPolicy {
  std::vector<std::int64_t> pdfParentItemIDs;

public:
  using const_iterator = std::vector<std::int64_t>::const_iterator;

  template <typename Iter>
  void operator()(Iter begin, Iter end)
  {
    pdfParentItemIDs.reserve(std::distance(begin, end));
    collect_parent_ids<PDFItem>(begin, end, std::back_inserter(pdfParentItemIDs));
  }

  [[nodiscard]] const_iterator cbegin() const { return pdfParentItemIDs.cbegin(); }
  [[nodiscard]] const_iterator cend() const { return pdfParentItemIDs.cend(); }
};

template <typename Item, typename ForwardIter, typename OutputIter>
void collect_ids(ForwardIter first, ForwardIter last, OutputIter outPutIter)
{
  if constexpr (std::is_same_v<Item, PDFItem>)
  {
    std::transform(first, last, outPutIter, [](const PDFItem& item) { return item.pdfAttachment.itemID; });
  }
  else if constexpr (std::is_same_v<Item, ZoteroCollection>)
  {
    std::transform(first, last, outPutIter, [](const ZoteroCollection& collection) { return collection.collectionID; });
  }
}

template <typename Item, typename ForwardIter, typename OutputIter>
void collect_parent_ids(ForwardIter first, ForwardIter last, OutputIter outPutIter)
{
  if constexpr (std::is_same_v<Item, PDFItem>)
  {
    std::transform(first, last, outPutIter, [](const PDFItem& item) { return item.pdfAttachment.parentItemID; });
  }
  else if constexpr (std::is_same_v<Item, ZoteroCollection>)
  {
    std::transform(first, last, outPutIter, [](const ZoteroCollection& collection) { return collection.parentCollectionID; });
  }
}

std::string_view standard_zotero_db_name()
{
  static constexpr std::string_view zotero_db_name = "zotero.sqlite";
  return zotero_db_name;
}

ZoteroDBInfo zotero_db_info(const std::filesystem::path& zotero_db_file_path)
{
  if (!std::filesystem::exists(zotero_db_file_path))
  {
    fmt::print("Zotero DB file does not exist: {}\n", zotero_db_file_path.string());
    std::abort();
  }

  ZoteroDBInfo zotero_db_info;
  try
  {
    // Open a database file in create/write mode
    SQLite::Database db(zotero_db_file_path, SQLite::OPEN_READONLY);

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

std::string formatted_zotero_db_info(const ZoteroDBInfo& info)
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

std::vector<ZoteroPDFAttachment> pdf_attachments(const std::filesystem::path& zotero_db_path)
{
  static const std::string queryString = fmt::format(
      "SELECT \n"
      "itemAttachments.itemID,\n"
      "itemAttachments.parentItemID,\n"
      "itemAttachments.path,\n"
      "items.key\n"
      "FROM \n"
      "itemAttachments \n"
      "LEFT JOIN items\n"
      "ON items.itemID = itemAttachments.itemID\n"
      "WHERE itemAttachments.contentType = 'application/pdf'");

  std::vector<ZoteroPDFAttachment> pdf_items;
  try
  {
    const SQLite::Database db(zotero_db_path, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString);

    while (query.executeStep())
    {
      std::int64_t parentItemID = -1;
      if (!query.isColumnNull(1))
      {
        parentItemID = query.getColumn(1).getInt64();
      }

      pdf_items.emplace_back(query.getColumn(0).getInt64(), parentItemID, query.getColumn(2).getText(), query.getColumn(3).getText());
    }
  }
  catch (std::exception& e)
  {
    fmt::print("SQLite exception: {}\n", std::string(e.what()));
    std::abort();
  }
  return pdf_items;
}

std::set<ZoteroCollection> parent_collections(const std::set<std::int64_t>& collectionIds, const std::filesystem::path& zotero_db_path)
{
  const std::string queryString = fmt::format(
      "SELECT c.collectionID, c.parentCollectionID, c.collectionName\n"
      "FROM collections c\n"
      "WHERE c.collectionID IN ({})",
      fmt::join(collectionIds.begin(), collectionIds.end(), ","));

  std::set<ZoteroCollection> result;
  try
  {
    const SQLite::Database db(zotero_db_path, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString);
    while (query.executeStep())
    {
      std::int64_t collectionID = -1;
      if (!query.isColumnNull(0))
      {
        collectionID = query.getColumn(0).getInt64();
      }

      std::int64_t parentCollectionID = -1;
      if (!query.isColumnNull(1))
      {
        parentCollectionID = query.getColumn(1).getInt64();
      }

      if (auto iter = collectionIds.find(collectionID) != collectionIds.end())
      {
        result.insert(ZoteroCollection{collectionID, parentCollectionID, query.getColumn(2).getText()});
      }
    }
  }
  catch (std::exception& e)
  {
    fmt::print("SQLite exception: {}\n", std::string(e.what()));
    std::abort();
  }

  return result;
}

std::vector<PDFItem> pdf_items(const std::vector<zotfiles::ZoteroPDFAttachment>& pdfAttachments, const std::filesystem::path& zoteroDbPath)
{
  std::vector<PDFItem> pdfItems;
  pdfItems.reserve(pdfAttachments.size());
  std::transform(pdfAttachments.begin(),
                 pdfAttachments.end(),
                 std::back_inserter(pdfItems),
                 [](const zotfiles::ZoteroPDFAttachment& pdfAttachment) { return PDFItem{pdfAttachment}; });

  std::vector<std::filesystem::path> pdfFiles;
  const std::string_view pdfItemPathPrefix = "storage:";
  for (auto& item: pdfItems)
  {

    if (item.pdfAttachment.path.find(pdfItemPathPrefix) == 0)
    {
      item.pdfAttachment.path = item.pdfAttachment.path.substr(pdfItemPathPrefix.size());
    }

    const std::filesystem::path storageDir = zoteroDbPath.parent_path() / "storage" / item.pdfAttachment.key;
    if (!std::filesystem::exists(storageDir))
    {
      continue;
    }

    pdfFiles.clear();
    for (const auto& file: std::filesystem::directory_iterator(storageDir))
    {
      if (file.path().extension() == ".pdf")
      {
        pdfFiles.push_back(file.path());
      }
    }
    if (pdfFiles.empty())
    {
      continue;
    }
    if (pdfFiles.size() > 1)
    {
      fmt::print("More than one pdf file found in the folder: {}\n", storageDir.string());
      continue;
    }

    item.pdfFilePath = pdfFiles.front();
  }

  return pdfItems;
}

template <typename ItemIDsPolicy, typename ForwardIter>
static std::unordered_map<std::int64_t, std::vector<ZoteroCollection>>
retrieve_item_collections(ForwardIter begin, ForwardIter end, const std::filesystem::path& zoteroDbPath)
{
  ItemIDsPolicy itemIDsPolicy;
  itemIDsPolicy(begin, end);
  const std::string queryString = item_collections_query(itemIDsPolicy.cbegin(), itemIDsPolicy.cend());

  std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> itemCollectionMap;
  try
  {
    const SQLite::Database db(zoteroDbPath, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString);

    while (query.executeStep())
    {
      if (query.isColumnNull(1))
      {
        continue;
      }

      std::int64_t parentCollectionID = -1;
      if (!query.isColumnNull(2))
      {
        parentCollectionID = query.getColumn(2).getInt64();
      }

      itemCollectionMap[query.getColumn(0).getInt64()].emplace_back(query.getColumn(1).getInt64(),
                                                                    parentCollectionID,
                                                                    query.getColumn(3).getString());
    }
  }
  catch (std::exception& e)
  {
    fmt::print("SQLite exception: {}\n", std::string(e.what()));
    std::abort();
  }

  return itemCollectionMap;
}

void retrieve_pdf_item_collections(std::vector<PDFItem>& pdfItems, const std::filesystem::path& zotero_db_path)
{
  // Find collections of the pdf items.
  const std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> itemCollectionMap =
      retrieve_item_collections<PDFItemIDsPolicy>(pdfItems.begin(), pdfItems.end(), zotero_db_path);

  std::for_each(std::execution::parallel_unsequenced_policy{},
                pdfItems.begin(),
                pdfItems.end(),
                [&itemCollectionMap](auto& pdfItem)
                {
                  auto iter = itemCollectionMap.find(pdfItem.pdfAttachment.itemID);
                  if (iter != itemCollectionMap.end())
                  {
                    pdfItem.collectionItems = iter->second;
                  }
                });

  // PDF items without a collection but a parent item id.
  auto noCollectionEndIter = std::partition(std::execution::parallel_unsequenced_policy{},
                                            pdfItems.begin(),
                                            pdfItems.end(),
                                            [](const PDFItem& pdfItem) { return pdfItem.collectionItems.empty(); });

  // Find collections of the parent items.
  const std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> parentItemMap =
      retrieve_item_collections<PDFParentItemIdsPolicy>(pdfItems.begin(), noCollectionEndIter, zotero_db_path);

  // Add the collections of the parent items to the pdf items.
  std::for_each(pdfItems.begin(),
                noCollectionEndIter,
                [&parentItemMap](PDFItem& pdfItem)
                {
                  auto collectionIter = parentItemMap.find(pdfItem.pdfAttachment.parentItemID);
                  if (collectionIter != parentItemMap.end())
                  {
                    pdfItem.collectionItems = collectionIter->second;
                  }
                });
}
std::unordered_map<std::int64_t, ZoteroCollection> all_pdf_item_collections(const std::vector<PDFItem>& pdfItems,
                                                                            const std::filesystem::path& zoteroDBPath)
{
  std::unordered_map<std::int64_t, ZoteroCollection> collectionMap;
  std::for_each(pdfItems.begin(),
                pdfItems.end(),
                [&collectionMap](const PDFItem& pdfItem)
                {
                  for (const auto& collection: pdfItem.collectionItems)
                  {
                    collectionMap.try_emplace(collection.collectionID, collection);
                  }
                });

  std::set<std::int64_t> missingParentCollections;
  for (const auto& pdfItem: pdfItems)
  {
    for (const auto& collection: pdfItem.collectionItems)
    {
      if (collection.parentCollectionID == -1)
      {
        continue;
      }

      auto iter = collectionMap.find(collection.parentCollectionID);
      if (iter == collectionMap.end())
      {
        missingParentCollections.emplace(collection.parentCollectionID);
      }
    }
  }

  while (!missingParentCollections.empty())
  {
    auto res = parent_collections(missingParentCollections, zoteroDBPath);
    missingParentCollections.clear();
    for (const auto& collection: res)
    {
      const std::int64_t collectionID = collection.collectionID;
      auto idIter = collectionMap.find(collectionID);
      if (idIter == collectionMap.end())
      {
        collectionMap.try_emplace(collectionID, collection);
      }

      const std::int64_t parentCollectionID = collection.parentCollectionID;
      auto parentIDIter = collectionMap.find(parentCollectionID);
      if (parentIDIter == collectionMap.end())
      {
        missingParentCollections.emplace(parentCollectionID);
      }
    }
  }

  return collectionMap;
}
} // namespace zotfiles
