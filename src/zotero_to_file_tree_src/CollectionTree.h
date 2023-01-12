#ifndef ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H
#define ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H

#include <cassert>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <utility>

namespace zotfiles
{

// The collection item represents a pdf item from the zotero db.
struct CollectionPDFItem
{
  std::int64_t pdfItemId{};
  std::string pdfName;
  std::filesystem::path pdfFilePath;
};

// A tree node representing a collection of pdf items.
// A CollectionNode must have a unique id.
struct CollectionNode
{
  std::int64_t collectionID{};
  std::int64_t parentCollectionID{};
  std::string collectionName;
  std::vector<CollectionPDFItem> collectionPDFItems;
  std::vector<std::shared_ptr<CollectionNode>> childrenNodes;

  bool operator==(const CollectionNode& rhs) const { return collectionID == rhs.collectionID; }
  bool operator!=(const CollectionNode& rhs) const { return !(rhs == *this); }
};

// Represents a tree of collections from the zotero db. Each collection contains the collection's pdf items.
class CollectionTree {
  std::vector<std::shared_ptr<CollectionNode>> m_collectionNodes;

public:
  // Build the collection for a given set of collection nodes.
  static CollectionTree build(std::unordered_map<std::int64_t, std::shared_ptr<CollectionNode>> collectionNodes);

  std::shared_ptr<CollectionNode> find(std::int64_t collectionID) const;

  // Write the pdf items to the given output directory with a directory tree structure matching the collection tree.
  std::size_t write_pdfs(std::filesystem::path outputDir, bool skipExistingFiles);

private:
  static bool erase_collection_node(std::vector<std::shared_ptr<CollectionNode>>& collectionNodes, const CollectionNode& collectionNode);
  bool remove(const CollectionNode& collectionNode);
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H
