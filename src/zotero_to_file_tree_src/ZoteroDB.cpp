#include "ZoteroDB.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <execution>
#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <unordered_map>

namespace zotfiles
{

static void insertDBValue(ZoteroDBInfo& info, const std::string_view key, std::int32_t val) {
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

class PDFItemIDsPolicy {
  std::vector<std::int64_t> pdfItemIDs;

public:
  using const_iterator = std::vector<std::int64_t>::const_iterator;

  template <typename Iter>
  void operator()(Iter begin, Iter end) {
    pdfItemIDs.reserve(static_cast<std::size_t>(std::distance(begin, end)));
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
  void operator()(Iter begin, Iter end) {
    pdfParentItemIDs.reserve(static_cast<std::size_t>(std::distance(begin, end)));
    collect_parent_ids<PDFItem>(begin, end, std::back_inserter(pdfParentItemIDs));
  }

  [[nodiscard]] const_iterator cbegin() const { return pdfParentItemIDs.cbegin(); }
  [[nodiscard]] const_iterator cend() const { return pdfParentItemIDs.cend(); }
};

template <typename Item, typename ForwardIter, typename OutputIter>
void collect_ids(ForwardIter first, ForwardIter last, OutputIter outPutIter) {
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
void collect_parent_ids(ForwardIter first, ForwardIter last, OutputIter outPutIter) {
  if constexpr (std::is_same_v<Item, PDFItem>)
  {
    std::transform(first, last, outPutIter, [](const PDFItem& item) { return item.pdfAttachment.parentItemID; });
  }
  else if constexpr (std::is_same_v<Item, ZoteroCollection>)
  {
    std::transform(first, last, outPutIter, [](const ZoteroCollection& collection) { return collection.parentCollectionID; });
  }
}

std::string_view standard_zotero_db_name() {
  static constexpr std::string_view zotero_db_name = "zotero.sqlite";
  return zotero_db_name;
}

ZoteroDBInfo zotero_db_info(const std::filesystem::path& zoteroDBPath) {
  if (!std::filesystem::exists(zoteroDBPath))
  {
    fmt::print("Zotero DB file does not exist: {}\n", zoteroDBPath.string());
    std::abort();
  }

  ZoteroDBInfo zotero_db_info;
  try
  {
    // Open a database file in create/write mode
    SQLite::Database db(zoteroDBPath, SQLite::OPEN_READONLY);

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

std::string formatted_zotero_db_info(const ZoteroDBInfo& info) {
  std::string table;
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

ZoteroDBInfo supported_zotero_db_info() {
  static constexpr ZoteroDBInfo supported_zotero_db_info{
      121,        // userdata
      18,         // triggers
      1682165479, // translators
      32,         // system
      1682165479, // styles
      1688765984, // repository
      28,         // globalSchema
      74,         // delete
      7           // compatibility;
  };
  return supported_zotero_db_info;
}

bool is_supported_zotero_db(const std::filesystem::path& zoteroDBPath) {
  auto zoteroDBInfo = zotero_db_info(zoteroDBPath);
  auto supportedZoteroDBInfo = supported_zotero_db_info();

  if (zoteroDBInfo.userdata != supportedZoteroDBInfo.userdata)
  {
    fmt::print("The zotero library is not supported: userdata version mismatch.\n");
    fmt::print("Found userdata: {}; Expected userdata: {}\n", zoteroDBInfo.userdata, supportedZoteroDBInfo.userdata);
    return false;
  }

  if (zoteroDBInfo.globalSchema != supportedZoteroDBInfo.globalSchema)
  {
    fmt::print("The zotero library is not supported: globalSchema version mismatch.\n");
    fmt::print("Found globalSchema: {}; Expected globalSchema: {}\n", zoteroDBInfo.globalSchema, supportedZoteroDBInfo.globalSchema);
    return false;
  }

  return true;
}

std::vector<ZoteroPDFAttachment> pdf_attachments(const std::filesystem::path& zoteroDBPath) {
  static std::string_view queryString = R"(
    SELECT
    itemAttachments.itemID,
    itemAttachments.parentItemID,
    itemAttachments.path,
    items.key
    FROM
    itemAttachments
    LEFT JOIN items ON items.itemID = itemAttachments.itemID
    WHERE itemAttachments.contentType = 'application/pdf')";

  std::vector<ZoteroPDFAttachment> pdf_items;
  try
  {
    const SQLite::Database db(zoteroDBPath, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString.data());

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

std::set<ZoteroCollection> parent_collections(const std::set<std::int64_t>& collectionIds, const std::filesystem::path& zoteroDBPath) {
  // Prepare the base query with placeholders
  std::string queryString =
      "SELECT c.collectionID, c.parentCollectionID, c.collectionName\n"
      "FROM collections c\n"
      "WHERE c.collectionID IN (";
  for (size_t i = 0; i < collectionIds.size(); ++i)
  {
    queryString += (i == 0 ? "?" : ",?");
  }
  queryString += ")";

  std::set<ZoteroCollection> result;
  try
  {
    const SQLite::Database db(zoteroDBPath, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString.data());

    // Bind values to the placeholders
    int count = 1;
    for (auto iter = collectionIds.begin(); iter != collectionIds.end(); ++iter)
    {
      query.bind(count, *iter);
      ++count;
    }

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

      // maybe_unused, because std::set has no contains
      if ([[maybe_unused]] auto iter = collectionIds.find(collectionID) != collectionIds.end())
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

std::vector<PDFItem> pdf_items(const std::vector<zotfiles::ZoteroPDFAttachment>& pdfAttachments,
                               const std::filesystem::path& zoteroDBPath) {
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

    const std::filesystem::path storageDir = zoteroDBPath.parent_path() / "storage" / item.pdfAttachment.key;
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
retrieve_item_collections(ForwardIter begin, ForwardIter end, const std::filesystem::path& zoteroDbPath) {
  ItemIDsPolicy itemIDsPolicy;
  itemIDsPolicy(begin, end);

  // Prepare the base query with placeholders
  std::string queryString = R"(
        SELECT
        items.itemID,
        collectionItems.collectionID,
        collections.parentCollectionID,
        collections.collectionName
        FROM
        items
        LEFT JOIN collectionItems ON collectionItems.itemID = items.itemID
        LEFT JOIN collections ON collections.collectionID = collectionItems.collectionID
        WHERE items.itemID IN ()";
  int count = 0;
  for (auto iter = itemIDsPolicy.cbegin(); iter != itemIDsPolicy.cend(); ++iter)
  {
    queryString += (count == 0 ? "?" : ",?");
    ++count;
  }
  queryString += ")";

  std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> itemCollectionMap;
  try
  {
    const SQLite::Database db(zoteroDbPath, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, queryString);

    int placeholderIndex = 1;
    for (auto it = itemIDsPolicy.cbegin(); it != itemIDsPolicy.cend(); ++it)
    {
      query.bind(placeholderIndex++, *it);
    }

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

void retrieve_pdf_item_collections(std::vector<PDFItem>& pdfItems, const std::filesystem::path& zoteroDBPath) {
  // Find collections of the pdf items.
  const std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> itemCollectionMap =
      retrieve_item_collections<PDFItemIDsPolicy>(pdfItems.begin(), pdfItems.end(), zoteroDBPath);

  std::for_each(std::execution::par_unseq,
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
  auto noCollectionEndIter = std::partition(std::execution::par_unseq,
                                            pdfItems.begin(),
                                            pdfItems.end(),
                                            [](const PDFItem& pdfItem) { return pdfItem.collectionItems.empty(); });

  // Find collections of the parent items.
  const std::unordered_map<std::int64_t, std::vector<ZoteroCollection>> parentItemMap =
      retrieve_item_collections<PDFParentItemIdsPolicy>(pdfItems.begin(), noCollectionEndIter, zoteroDBPath);

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
                                                                            const std::filesystem::path& zoteroDBPath) {
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
