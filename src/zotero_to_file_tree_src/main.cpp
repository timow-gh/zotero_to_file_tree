#include "CLI/CLI.hpp"
#include <iostream>

int main(int argc, char** argv)
{
  CLI::App app{"Converts a exported zotero library to a file tree."};

  std::string zotero_exported_library_path = "";
  app.add_option("-zotero_lib", zotero_exported_library_path, "Path to the exported zotero library.")->required();

  CLI11_PARSE(app, argc, argv);

  std::cout << "zotero_exported_library_path value: " << zotero_exported_library_path << std::endl;
  return 0;
}