# zotero_to_file_tree

zotero_to_file_tree extracts the pdf attachments from a Zotero library and copies them to a file tree.

## Usage

```
zotero_to_file_tree.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -l,--lib <path>             Path to the zotero library. Default is the current directory.
  -o,--output_dir <path>      Path to the output directory.
  --print_db_info             Print the zotero db info.
  --overwrite_all             Overwrite the output directory if it exists.
  --skip_existing             Skip existing files if they exist in the output directory.
```