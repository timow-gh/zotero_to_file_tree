#ifndef ZOTERO_TO_FILE_TREE_PDFITEM_H
#define ZOTERO_TO_FILE_TREE_PDFITEM_H

#include "PDFAttachment.h"
#include "ZoteroCollection.h"
#include <filesystem>
#include <vector>

namespace zotfiles
{

struct PDFItem
{
  PDFAttachment pdfAttachment;
  std::filesystem::path pdfFilePath;
  std::vector<ZoteroCollection> collectionItems;
};

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_PDFITEM_H
