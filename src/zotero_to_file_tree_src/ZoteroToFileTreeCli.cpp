#include "ZoteroToFileTree.h"

int main(int argc, char** argv)
{
  std::error_code errorCode = zotfiles::ZoteroToFileTree::run(argc, argv);
  return errorCode.value() == 0 ? 0 : 1;
}