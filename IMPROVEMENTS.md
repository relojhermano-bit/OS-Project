# Improvements and Verification Notes

This document records the final improvements made after checking the project
against the operating systems assignment requirements.

## Code Improvements

1. Input retry handling is safer.
   - Invalid user input still prints a warning and asks again.
   - The input buffer cleanup now handles `EOF`, so the program will not loop
     forever if input ends unexpectedly.

2. Logical address reading is stricter.
   - `readLogicalAddress()` now reports three states: valid address, end of file,
     and invalid address.
   - Addresses outside the 16-bit logical address space, such as negative values
     or values greater than 65535, are rejected instead of being silently cast to
     `unsigned short`.

3. Backing store reads are checked.
   - `loadPage()` now checks whether `fseek()` succeeds.
   - It also verifies that `fread()` reads exactly one page.
   - If a page cannot be loaded correctly, the program exits with an error.

4. Compatibility was improved.
   - Local variables used in the main translation loop were moved to the top of
     the function body.
   - The code now compiles cleanly with both the normal Makefile flags and an
     additional strict C89 check.

5. Documentation was updated.
   - `README.md` now includes build and run commands.
   - `PROJECT_REPORT.md` now describes the Ubuntu Linux / GCC validation
     environment.

## Requirement Check

- Uses a 16-bit logical address space of 65536 bytes.
- Reads logical addresses from `addresses.txt`.
- Reads pages from `backingstore.bin` with `fopen()`, `fseek()`, `fread()`, and
  `fclose()`.
- Uses arrays for the page table and simulated physical memory.
- Starts with pure demand paging.
- Allocates free frames from low frame number to high frame number.
- Uses LRU page replacement when physical memory is full.
- Prints page loads, page replacements, page table hits, and address
  translations in the required format.
- Generates `stat.txt` with page-fault rate and memory image.
- Includes only `stdio.h`, `stdlib.h`, and the project header file.

## Verification Commands

```bash
make clean && make
```

Result: passed with no warnings.

```bash
gcc -std=c89 -Wall -Wextra -pedantic -c main.c
gcc -std=c89 -Wall -Wextra -pedantic -c mmu.c
```

Result: passed with no warnings.

```bash
printf "10\n32\n" | ./vm_manager > my_output.txt
diff output.txt my_output.txt
```

Result: no differences.

Additional invalid input checks were also run:

```bash
printf "10\n33\n10\n32\n" | ./vm_manager > /tmp/invalid_then_valid_output.txt
printf "10\n64\n10\n32\n" | ./vm_manager > /tmp/invalid_equal_mem_output.txt
```

Both cases printed a warning and then accepted the corrected input.
