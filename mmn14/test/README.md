# Final Project - 32-bit Assembler
By Tzur Pinto Lazar

This project is a 2-pass assembler for a 32-bit architecture, supporting R, I, and J instruction types. It includes a macro preprocessing stage and generates hexadecimal little-endian machine code.

## Files in the Project

### assembler.c
The main engine of the program. It coordinates the preprocessing and the two passes.
- process_file: Runs the whole assembly lifecycle for a given file name.
- write_output: Generates the .ob, .ent, and .ext files with the correct formatting.
- free_code / free_data: Cleanup functions for the linked lists.

### preproc.c
Handles the macro expansion stage before the actual assembly begins.
- preprocess: Reads the .as file and creates a .am file with expanded macros. It uses the mcro and mcroend keywords.

### first_pass.c
Performs the first scan of the assembly code.
- first_pass: Collects all labels into the symbol table, calculates instruction (IC) and data (DC) sizes, and performs the initial encoding of machine words.
- add_code_node / add_data_node: Helpers to build the linked lists representing memory.

### second_pass.c
Performs the second scan to resolve addresses.
- second_pass: Handles .entry definitions and fills in the missing label addresses or branch offsets in the machine words collected during the first pass.
- add_ext_usage: Tracks where external symbols are used for the .ext file.

### table.c
Implements the data structures used by the assembler.
- add_symbol / get_symbol: Logic for the Symbol Table (labels).
- add_macro / get_macro_content: Logic for the Macro Table.
- free_symbols / free_macros: Memory management for the tables.

### util.c
General utility functions recycled from previous assignments.
- safe_malloc / safe_realloc: Memory allocation with failure checks.
- skipSpaces: Skips whitespace characters in a string.
- get_token / get_comma_token: String parsing utilities.
- get_reg_num: Converts register strings (e.g., "$5") to integers.

### globals.h
Contains all the global definitions, including opcodes, instruction types, and architectural constants like the word size and initial IC.

### Makefile
Standard build script to compile the project with -Wall -ansi -pedantic.

## Build and Usage

1. Build: run 'make'
2. Run: run './assembler filename' (without extension)
3. Clean: run 'make clean'
