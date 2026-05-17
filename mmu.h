#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdlib.h>

#define LOGICAL_ADDRESS_BITS 16
#define LOGICAL_SPACE_SIZE 65536

/* Page table entry structure */
typedef struct {
    int valid;       /* 1 if page is loaded in a frame */
    int frameNo;     /* frame number where page is loaded */
    long lastUsed;   /* timestamp for LRU */
} PageTableEntry;

/* Function prototypes */

/* Input validation */
int isPowerOfTwo(int x);
int getValidInput(int *frameBits, int *frameCount);

/* Address translation */
int getPageNo(unsigned short logicalAddress, int frameBits);
int getOffset(unsigned short logicalAddress, int frameBits);
unsigned short readLogicalAddress(FILE *fp);

/* Frame management */
int findFreeFrame(int *frameToPage, int frameCount);
int findLRUFrame(long *frameLastUsed, int frameCount);
void loadPage(FILE *backingStore, unsigned char *memory, 
              int pageNo, int frameNo, int pageSize);

/* Statistics */
void writeStat(int *frameToPage, int pageFaultCount, 
              int totalAddressCount, int frameCount);

#endif
