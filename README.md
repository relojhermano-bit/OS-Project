# OS Virtual Memory Management Project

This project implements a virtual memory management simulator in C.

The program reads logical addresses from `addresses.txt`, translates them into physical addresses, loads pages from `backingstore.bin` when page faults occur, and uses the LRU page replacement algorithm when physical memory is full.

## Files

- `main.c` - Main program
- `mmu.c` - Memory management implementation
- `mmu.h` - Header file
- `addresses.txt` - Input logical addresses
- `backingstore.bin` - Backing store file
- `Makefile` - Build file
- `output.txt` - Program output
- `stat.txt` - Statistics output
- `PROJECT_REPORT.md` - Project report
- `IMPROVEMENTS.md` - Final improvement and verification notes

## Build

```bash
make
```

## Run

Run the program from the project directory so it can find `addresses.txt` and
`backingstore.bin`.

```bash
./vm_manager > output.txt
```

The program asks for the frame size bits and the total frame count. For example:

```bash
printf "10\n32\n" | ./vm_manager > output.txt
```

The run also generates `stat.txt`.

## Clean

```bash
make clean
```

## Features

- Address translation
- Page fault handling
- Demand paging
- LRU page replacement
- Page fault rate output
- Memory image output
