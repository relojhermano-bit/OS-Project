#include "mmu.h"

/* Check if x is a power of 2 */
int isPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}

/* Get and validate user inputs for frame size bits and frame count */
int getValidInput(int *frameBits, int *frameCount) {
    fprintf(stderr, "Enter frame size bits (n): ");
    if (scanf("%d", frameBits) != 1) {
        fprintf(stderr, "Warning: Invalid input for frame size bits.\n");
        return 0;
    }

    fprintf(stderr, "Enter total frame count: ");
    if (scanf("%d", frameCount) != 1) {
        fprintf(stderr, "Warning: Invalid input for frame count.\n");
        return 0;
    }

    /* Validate frame size bits: 1 <= frameBits <= 15 */
    if (*frameBits < 1 || *frameBits > 15) {
        fprintf(stderr, "Warning: Frame size bits must be between 1 and 15.\n");
        return 0;
    }

    int pageSize = 1 << *frameBits;
    int physicalMemorySize = *frameCount * pageSize;

    /* Validate frame count is positive */
    if (*frameCount < 1) {
        fprintf(stderr, "Warning: Frame count must be positive.\n");
        return 0;
    }

    /* Validate physical memory size is smaller than logical space */
    if (physicalMemorySize >= LOGICAL_SPACE_SIZE) {
        fprintf(stderr, "Warning: Physical memory size (%d bytes) must be smaller than logical address space (65536 bytes).\n",
                physicalMemorySize);
        return 0;
    }

    /* Validate physical memory size is power of 2 */
    if (!isPowerOfTwo(physicalMemorySize)) {
        fprintf(stderr, "Warning: Total physical memory size (%d bytes) must be a power of 2.\n",
                physicalMemorySize);
        return 0;
    }

    return 1;
}

/* Extract page number from logical address */
int getPageNo(unsigned short logicalAddress, int frameBits) {
    return logicalAddress >> frameBits;
}

/* Extract offset from logical address */
int getOffset(unsigned short logicalAddress, int frameBits) {
    int pageSize = 1 << frameBits;
    return logicalAddress & (pageSize - 1);
}

/* Read next logical address from input file */
unsigned short readLogicalAddress(FILE *fp) {
    int addr;
    if (fscanf(fp, "%d", &addr) == 1) {
        return (unsigned short)addr;
    }
    return (unsigned short)-1; /* EOF marker */
}

/* Find the lowest-index free frame */
int findFreeFrame(int *frameToPage, int frameCount) {
    int i;
    for (i = 0; i < frameCount; i++) {
        if (frameToPage[i] == -1) {
            return i;
        }
    }
    return -1; /* No free frame */
}

/* Find the LRU victim frame (smallest lastUsed timestamp among loaded frames) */
int findLRUFrame(long *frameLastUsed, int frameCount) {
    int victim = 0;
    int i;
    /* Find first loaded frame */
    for (i = 0; i < frameCount; i++) {
        if (frameLastUsed[i] != -1) {
            victim = i;
            break;
        }
    }
    /* Find frame with smallest timestamp */
    for (i = victim + 1; i < frameCount; i++) {
        if (frameLastUsed[i] != -1 && frameLastUsed[i] < frameLastUsed[victim]) {
            victim = i;
        }
    }
    return victim;
}

/* Load a page from backing store into memory */
void loadPage(FILE *backingStore, unsigned char *memory, 
              int pageNo, int frameNo, int pageSize) {
    int offset = pageNo * pageSize;
    size_t bytesRead;
    
    fseek(backingStore, offset, SEEK_SET);
    bytesRead = fread(memory + frameNo * pageSize, sizeof(unsigned char), pageSize, backingStore);
    
    if ((int)bytesRead != pageSize) {
        fprintf(stderr, "Warning: Read %zu bytes, expected %d bytes for page %d.\n",
                bytesRead, pageSize, pageNo);
    }
}

/* Write stat.txt with page-fault rate and memory image */
void writeStat(int *frameToPage, int pageFaultCount, 
              int totalAddressCount, int frameCount) {
    FILE *fp = fopen("stat.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create stat.txt\n");
        return;
    }

    /* Calculate page-fault rate as decimal ratio */
    double faultRate = (totalAddressCount > 0) ? (double)pageFaultCount / totalAddressCount : 0.0;
    fprintf(fp, "page-fault rate: %.6f\n", faultRate);
    fprintf(fp, "Memory image:\n");

    /* Print memory image: 16 frames per row */
    int i;
    for (i = 0; i < frameCount; i++) {
        if (i % 16 == 0) {
            if (i > 0) {
                fprintf(fp, "\n");
            }
            fprintf(fp, "Frame %d ~ Frame %d: ", i, 
                    (i + 15 < frameCount) ? i + 15 : frameCount - 1);
        }
        fprintf(fp, "%d", frameToPage[i]);
        if (i % 16 != 15 && i != frameCount - 1) {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, "\n");

    fclose(fp);
}
