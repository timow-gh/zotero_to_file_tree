# Zotero to File Tree

zotero_to_file_tree.exe extracts the pdf files from a Zotero library and copies them to a directory structure based on the
Zotero collection structure.

## Usage

```
zotero_to_file_tree.exe [OPTIONS]

Options:
    -h,--help                 Print this help message and exit
    -l,--lib <path>           Path to the zotero library. Default is the current directory.
    -o,--output_dir <path>    Path to the output directory.
    --print_db_info           Print the zotero db info.
    --overwrite_output_dir    Overwrite the output directory if it exists. 
                              All files in the output directory will be deleted.
    --skip_existing           Skip existing files if they exist in the output directory.
```