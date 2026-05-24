#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdlib.h>

#define LOGICAL_ADDRESS_BITS 16
#define LOGICAL_SPACE_SIZE 65536
#define READ_ADDRESS_INVALID -1
#define READ_ADDRESS_EOF 0
#define READ_ADDRESS_OK 1

/* Page table entry structure */
typedef struct {
    int valid;       /* 1 if page is loaded in a frame */
    int frameNo;     /* frame number where page is loaded */
    long lastUsed;   /* timestamp for LRU */
} PageTableEntry;

/* Function prototypes */

/* Input validation */
int isPowerOfTwo(long x);
int getValidInput(int *frameBits, int *frameCount);

/* Address translation */
int getPageNo(unsigned short logicalAddress, int frameBits);
int getOffset(unsigned short logicalAddress, int frameBits);
int readLogicalAddress(FILE *fp, unsigned short *logicalAddress);

/* Frame management */
int findFreeFrame(int *frameToPage, int frameCount);
int findLRUFrame(long *frameLastUsed, int frameCount);
int loadPage(FILE *backingStore, unsigned char *memory,
             int pageNo, int frameNo, int pageSize);

/* Statistics */
void writeStat(int *frameToPage, int pageFaultCount, 
              int totalAddressCount, int frameCount);

#endif
