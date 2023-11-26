/*!
* \mainpage Zotero to file tree: Convert your Zotero library to a file tree
*
* \section intro_sec Introduction
*
* ZotFiles is a CLI application that converts a Zotero library to a file tree.
* All PDFs in the Zotero library are copied to the output directory and organized in a file tree.
* The file tree is organized by the collection names and sub-collections of the Zotero library.
*
* \section usage_sec Usage
*
* The general usage of the command is as follows:
*
* ```
* zotero_to_file_tree [options]
* ```
*
* \subsection options_sec Options
*
* | Short Option | Long Option    | Description   |
* |--------------|----------------|---------------|
* | -l           | -\-lib          | Path to the zotero library. Default is the current directory.|
* | -o           | -\-output_dir   | Path to the output directory. |
* | -\-print_db_info | | Print the zotero db info. |
* | -\-overwrite_dir | | Overwrite the output directory if it exists. |
* | -\-overwrite_files | | Overwrite existing files if they exist in the output directory. |
*
* \section example_sec Examples
*
* Print the zotero db info:
* ```
* zotero_to_file_tree --print_db_info
* ```
*
* Export PDFs and skip already existing files in the output directory:
* ```
* zotero_to_file_tree -l /path/to/library -o /path/to/output
* ```
* Replace the output directory if it exists. This will remove all files in the output directory:
* ```
* zotero_to_file_tree -l /path/to/library -o /path/to/output --overwrite_dir
* ```
*
* Add new files, replace files that already exist in the output directory. Keep all other files:
* ```
* zotero_to_file_tree -l /path/to/library -o /path/to/output --overwrite_files
* ```
*/