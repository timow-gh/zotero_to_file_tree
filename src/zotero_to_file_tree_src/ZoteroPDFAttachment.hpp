#ifndef ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H
#define ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H

#include <cstdint>
#include <string>

namespace zotfiles
{

/**
 *\brief Represents a zotero db entry that is marked to have a pdf attachment.
 *
 * A ZoteroPDFAttachment represents an item in the zotero db that may have one or more pdf files attached.
 */
struct ZoteroPDFAttachment {
  std::int64_t itemID{};       /**< The itemID is the primary key of the items table. */
  std::int64_t parentItemID{}; /**< -1 if the item has no parent item. */
  std::string path;            /**< Named after the 'path' entry in the zotero db. */
  std::string key;             /**< Named after the 'key' entry in the zotero sb. */
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H
