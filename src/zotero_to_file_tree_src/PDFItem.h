#ifndef ZOTERO_TO_FILE_TREE_PDFITEM_H
#define ZOTERO_TO_FILE_TREE_PDFITEM_H

#include "ZoteroCollection.h"
#include "ZoteroPDFAttachment.h"
#include <filesystem>
#include <vector>

namespace zotfiles
{

/*
 * @brief Represents a pdf file in the storage directory.
 *
 * @member pdfAttachment The entry in the zotero db that represents the pdf file.
 * @member pdfFilePath The absolute path to the pdf file on the file system.
 * @member collectionItems The zotero collections that the pdf file is in.
 */
struct PDFItem
{
  ZoteroPDFAttachment pdfAttachment;
  std::filesystem::path pdfFilePath;
  std::vector<ZoteroCollection> collectionItems;
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFITEM_H
