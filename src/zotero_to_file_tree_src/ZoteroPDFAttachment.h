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
 *
 * @member itemID The itemID is the primary key of the items table.
 * @member parentItemID The parentItemID is the itemID of the parent item.
 * @member path The path is the name of the pdf file stored.
 * @member key The key is the name of the subdirectory in the zotero storage directory, where pdf files for this itemID are stored.
 */
struct ZoteroPDFAttachment
{
  std::int64_t itemID{};
  std::int64_t parentItemID{};
  std::string path; // Named after the 'path' entry in the zotero db.
  std::string key;  // Named after the 'key' entry in the zotero sb.
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H
