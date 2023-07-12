#ifndef ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H
#define ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H

#include <cassert>
#include <compare>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <utility>

namespace zotfiles
{

/** @brief The collection item represents a pdf item from the zotero db.
 */
struct CollectionPDFItem
{
  std::int64_t pdfItemId{}; /**< Identifies the CollectionPDFItem uniquely. */
  std::string pdfName;               /**< The name of the pdf file. */
  std::filesystem::path pdfFilePath; /**< The absolute path to the pdf file. */

  std::strong_ordering operator<=>(const CollectionPDFItem& rhs) const { return pdfItemId <=> rhs.pdfItemId; }
};

/** @brief A tree node representing a collection containing pdf items.
 *
 * A CollectionNode must have a unique id.
 */
struct CollectionNode
{
  std::int64_t collectionID{}; /**< Identifies the CollectionNode uniquely. */
  std::int64_t parentCollectionID{}; /**< CollectionID of the parent. -1 if no parent exists. */
  std::string collectionName;
  std::vector<CollectionPDFItem> collectionPDFItems;
  std::vector<std::shared_ptr<CollectionNode>> childrenNodes;

  std::strong_ordering operator<=>(const CollectionNode& rhs) const { return collectionID <=> rhs.collectionID; }
};

/** @brief The collection tree as displayed by the zotero app
 *
 * The nodes of the collection tree represent the folders of the collections in the zotero app.
 * Only nodes containing pdf items are included in the tree.
 */
class CollectionTree {
  std::vector<std::shared_ptr<CollectionNode>> m_collectionNodes;

public:
  /** @brief  Build the collection for a given set of collection nodes.
   *
   */
  static CollectionTree build(std::unordered_map<std::int64_t, std::shared_ptr<CollectionNode>> collectionNodes);

  std::shared_ptr<CollectionNode> find(std::int64_t collectionID) const;

  /** @brief Write the pdfs to the output directory.
   *
   * Write the pdf items to the given output directory with a directory tree structure matching the collection tree.
   *
   *  @return {written, skipped} The number of pdf files written and skipped.
   */
  std::pair<std::size_t, std::size_t> write_pdfs(std::filesystem::path outputDir, bool overwriteExistingFiles);

private:
  static bool erase_collection_node(std::vector<std::shared_ptr<CollectionNode>>& collectionNodes, const CollectionNode& collectionNode);
  bool remove(const CollectionNode& collectionNode);
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_COLLECTIONTREE_H
