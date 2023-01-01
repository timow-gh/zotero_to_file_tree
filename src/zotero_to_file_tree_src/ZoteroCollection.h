#ifndef ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H
#define ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H

#include <cstdint>
#include <string>

namespace zotfiles
{

struct ZoteroCollection
{
  std::int64_t collectionID{};
  // -1 if the collection has no parent collection
  std::int64_t parentCollectionID{};
  std::string collectionName;

  auto operator<=>(const ZoteroCollection& other) const { return collectionID <=> other.collectionID; }
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTEROCOLLECTION_H
