#ifndef ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H
#define ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H

#include <cstdint>
#include <string>

namespace zotfiles
{

/**
 *\brief Represents a Zotero collection.
 *
 * Zotero collections are a tree structure that is displayed in the Zotero client.
 */
struct ZoteroCollection {
  std::int64_t collectionID{};       /**< collectionID The collectionID is the primary key of the collections table. */
  std::int64_t parentCollectionID{}; /**< -1 if the collection has no parent collection */
  std::string collectionName;

  auto operator<=>(const ZoteroCollection& other) const { return collectionID <=> other.collectionID; }
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H
