# Jarch Tools

This folder contains the core development tools for Jarch: the assembler, linker, emulator, and test utilities.

## Quick Start

- **Assemble a file**:  
  ```bash
  asm source.s obj.o
  ```
  Produces an object file from a single assembly source.

- **Link object files**:  
  ```bash
  lnk source1.o -o final.hex
  ```
  Combines object files into a hex file. Currently, cross-file labels are not supported, so multi-file projects without cross-references will work.

- **Run programs**:  
  Use the emulator to execute assembled or linked programs.

## Output

All executables and object files go in the `bin/` folder inside `Tools/`:

```
Jarch/
└── tools/
    ├── asm
    ├── lnk
    ├── emu
    ├── test/
    └── bin/
```

## Notes

- The assembler works for single files.  
- The linker can combine multiple files but cannot yet resolve labels across files.  

Future updates will add full multi-file support and expand emulator capabilities.
 clarity is welcome.
