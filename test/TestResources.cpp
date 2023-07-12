#include "TestResources.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

std::filesystem::path zotero_example_db_depr()
{
#ifdef RESOURCE_DIR_DEPR
  return std::filesystem::path(TOSTRING(RESOURCE_DIR_DEPR));
#else
  static_assert(false, "RESOURCE_DIR_DEPR is not defined");
#endif
}

std::filesystem::path zotero_example_db()
{
#ifdef RESOURCE_DIR
  return std::filesystem::path(TOSTRING(RESOURCE_DIR));
#else
  static_assert(false, "RESOURCE_DIR is not defined");
#endif
}
