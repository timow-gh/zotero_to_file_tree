#include "ErrorCodes.hpp"

namespace zotfiles
{

const char* ZoteroToFileTreeErrorCategory::name() const noexcept
{
  return "ZoteroToFileTreeErrorCategory";
}
std::string ZoteroToFileTreeErrorCategory::message(int ev) const
{
  ErrorCodes errorCode = static_cast<ErrorCodes>(ev);
  switch (errorCode)
  {
  case ErrorCodes::SUCCESS: return "Success";
  case ErrorCodes::ZOTERO_DB_DOES_NOT_EXIST: return "The zotero library path does not exist";
  case ErrorCodes::ZOTERO_DB_NOT_SUPPORTED: return "The zotero library path does not point to a supported zotero database";
  case ErrorCodes::OUTPUT_DIR_INVALID: return "The output directory path is not valid";
  default: return "Unknown ZoteroToFileTree error";
  }
}

std::error_code make_error_code(zotfiles::ErrorCodes errorCode)
{
  static zotfiles::ZoteroToFileTreeErrorCategory errorCategory;
  return {static_cast<int>(errorCode), errorCategory};
}

} // namespace zotfiles
