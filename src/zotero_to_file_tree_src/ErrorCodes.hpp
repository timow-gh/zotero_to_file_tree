#ifndef ZOTERO_TO_FILE_TREE_ERRORCODES_HPP
#define ZOTERO_TO_FILE_TREE_ERRORCODES_HPP

#include <system_error>

namespace zotfiles
{

enum class ErrorCodes
{
  SUCCESS = 0, /**< 0 must represent success for every error_category */
  CLI_PARSE_ERROR,
  ZOTERO_DB_DOES_NOT_EXIST,
  ZOTERO_DB_NOT_SUPPORTED,
  OUTPUT_DIR_INVALID
};

class ZoteroToFileTreeErrorCategory : public std::error_category {
public:
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};

std::error_code make_error_code(ErrorCodes errorCode);

} // namespace zotfiles

namespace std
{

template <>
struct is_error_code_enum<zotfiles::ErrorCodes> : true_type
{
};

} // namespace std

#endif // ZOTERO_TO_FILE_TREE_ERRORCODES_HPP
