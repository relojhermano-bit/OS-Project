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

## Build

```bash
make
```

## Run

```bash
./vm_manager > output.txt
```

Enter the frame size bits and the number of frames when prompted.

Example:

```text
10
32
```

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
