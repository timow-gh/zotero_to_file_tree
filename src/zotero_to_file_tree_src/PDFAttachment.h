#ifndef ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H
#define ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H

#include <cstdint>
#include <string>

namespace zotfiles
{

struct PDFAttachment
{
  std::int64_t itemID{};
  std::int64_t parentItemID{};
  std::string path; // member is named after path in the zotero db. This is not a filesystem path, but the name of the pdf file stored.
  std::string key;  // The key is the name of the subdirectory in the storage directory, where the pdf file is stored.
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFATTACHEMENT_H
