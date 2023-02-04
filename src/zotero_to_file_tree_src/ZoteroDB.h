#ifndef ZOTERO_TO_FILE_TREE_ZOTERODB_H
#define ZOTERO_TO_FILE_TREE_ZOTERODB_H

#include "PDFItem.h"
#include "ZoteroCollection.h"
#include <cstdint>
#include <filesystem>
#include <set>
#include <unordered_map>

namespace zotfiles
{

struct ZoteroDBInfo
{
  // schema = version;
  std::uint32_t userdata{};
  std::uint32_t triggers{};
  std::uint32_t translators{};
  std::uint32_t system{};
  std::uint32_t styles{};
  std::uint32_t repository{};
  std::uint32_t globalSchema{};
  std::uint32_t deletes{}; // original: delete, but delete is a keyword
  std::uint32_t compatibility{};
};

/*
 * @brief Returns the standard file name of the zotero db.
 */
[[nodiscard]] std::string_view standard_zotero_db_name();

/*
 * @brief Returns the zotero db info.
 *
 * Returns the zotero db info for the given zotero db file path or aborts if the zotero db file does not exist.
 *
 * @param zotero_db_file_path Absolute path to the zotero db file.
 *
 * @return The ZoteroDBInfo for the given path.
 */
[[nodiscard]] ZoteroDBInfo zotero_db_info(const std::filesystem::path& zotero_db_file_path);

/*
 * @brief Returns a formatted string of the given ZoteroDBInfo.
 *
 * @param info The ZoteroDBInfo to format.
 */
[[nodiscard]] std::string formatted_zotero_db_info(const ZoteroDBInfo& info);

/*
 * @brief Returns the supported ZoteroDBInfo.
 *
 * This is the ZoteroDBInfo that is supported by this program.
 */
[[nodiscard]] ZoteroDBInfo supported_zotero_db_info();

/*
 * @brief Returns true if the given zotero db info is supported.
 *
 * @param info The ZoteroDBInfo to check.
 *
 * @return True if the given zotero db info is supported.
 */
[[nodiscard]] bool is_supported_zotero_db(const std::filesystem::path& zotero_db_path);

/*
 * @brief Retrieves all pdf attachments from the zotero db.
 *
 * @param zotero_db_path Absolute path to the zotero db file.
 */
[[nodiscard]] std::vector<ZoteroPDFAttachment> pdf_attachments(const std::filesystem::path& zotero_db_path);

// All collections that are parents of the given collectionIDs that are not already in the given collections.
/*
 * @brief Retrieves all parent collections of the given collection IDs.
 *
 * @param collectionIds The collection IDs to retrieve the parent collections for.
 */
[[nodiscard]] std::set<ZoteroCollection> parent_collections(const std::set<std::int64_t>& collectionIds,
                                                            const std::filesystem::path& zotero_db_path);

/*
 * @brief Retrieves all pdf items from the given ZoteroPDFAttachments.
 *
 * A pdf item is a concrete pdf file found in the zotero storage directory. A ZoteroPDFAttachment is an entry in the zotero db that may
 * contain multiple pdf items. It is possible that no pdf files are found for a ZoteroPDFAttachment, because the pdf files were deleted
 * separately.
 *
 * @param pdfAttachments The pdf attachments to retrieve the pdf items for.
 * @param zotero_db_path Absolute path to the zotero db file.
 *
 * @return The found pdf files.
 */
[[nodiscard]] std::vector<PDFItem> pdf_items(const std::vector<zotfiles::ZoteroPDFAttachment>& pdfAttachments,
                                             const std::filesystem::path& zoteroDbPath);

/*
 * @brief Retrieves all collections of the given PDFItems and their parent collections.
 *
 * Most pdf items have a parent item that holds the information about the collection hierarchy.
 * This function adds the missing information to each PDFItem. That holds all relevant information about the pdf file and the collections
 * its belonging to. The PDFItems are modified in place.
 *
 * @param pdfItems The pdf items to retrieve the collections for.
 * @param zotero_db_path Absolute path to the zotero db file.
 */
void retrieve_pdf_item_collections(std::vector<PDFItem>& pdfItems, const std::filesystem::path& zotero_db_path);

/*
 * @brief Collects all pdf item collections and their parent collections
 *
 * @param pdfItems The pdf items to retrieve the collections for.
 * @param zotero_db_path Absolute path to the zotero db file.
 *
 * @return A map of collection IDs to ZoteroCollection.
 */
std::unordered_map<std::int64_t, ZoteroCollection> all_pdf_item_collections(const std::vector<PDFItem>& pdfItems,
                                                                            const std::filesystem::path& zoteroDBPath);

} // namespace zotfiles

#endif // ZOTERO_TO_FILE_TREE_ZOTERODB_H
