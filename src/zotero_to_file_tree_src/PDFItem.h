#ifndef ZOTERO_TO_FILE_TREE_PDFITEM_H
#define ZOTERO_TO_FILE_TREE_PDFITEM_H

#include "ZoteroCollection.h"
#include "ZoteroPDFAttachment.h"
#include <filesystem>
#include <vector>

namespace zotfiles
{

/**
 *\brief Represents a pdf file in the storage directory.
 */
struct PDFItem
{
  ZoteroPDFAttachment pdfAttachment; /**< pdfAttachment The entry in the zotero db that represents the pdf file. */
  std::filesystem::path pdfFilePath; /**< pdfFilePath The absolute path to the pdf file on the file system. */
  std::vector<ZoteroCollection> collectionItems; /**< collectionItems The zotero collections that the pdf file is in. */
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFITEM_H
