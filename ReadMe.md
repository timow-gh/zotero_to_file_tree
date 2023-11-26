# Zotero to File Tree

zotero_to_file_tree.exe extracts the pdf files from a Zotero library and copies them to a directory structure based on the
Zotero collection structure.

## Usage

```
zotero_to_file_tree.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -l,--lib TEXT               Path to the zotero library. Default is the current directory.
  -o,--output_dir TEXT        Path to the output directory.
  --print_db_info             Print the zotero db info.
  --overwrite_dir             Overwrite the output directory if it exists.
  --overwrite_files           Overwrite existing files if they exist in the output directory.
```