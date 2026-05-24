#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"

int main() {
    int frameBits, frameCount;
    int pageSize, pageCount, physicalMemorySize;
    PageTableEntry *pageTable;
    unsigned char *memory;
    int *frameToPage;
    long *frameLastUsed;
    FILE *addressFile, *backingStore;
    unsigned short logicalAddress;
    long accessCounter = 0;
    int pageFaultCount = 0;
    int pageNo;
    int offset;
    int frameNo;
    int physicalAddr;
    int freeFrame;
    int victimFrame;
    int oldPage;
    int readStatus;
    unsigned char data;
    int ch;
    int i;

    /* Get and validate user inputs */
    while (!getValidInput(&frameBits, &frameCount)) {
        fprintf(stderr, "Please try again.\n");
        /* Clear input buffer */
        do {
            ch = getchar();
        } while (ch != '\n' && ch != EOF);
        if (ch == EOF) {
            return 1;
        }
    }

    /* Derive system parameters */
    pageSize = 1 << frameBits;
    pageCount = 1 << (LOGICAL_ADDRESS_BITS - frameBits);
    physicalMemorySize = frameCount * pageSize;

    /* Allocate data structures */
    pageTable = (PageTableEntry *)malloc(pageCount * sizeof(PageTableEntry));
    memory = (unsigned char *)malloc(physicalMemorySize * sizeof(unsigned char));
    frameToPage = (int *)malloc(frameCount * sizeof(int));
    frameLastUsed = (long *)malloc(frameCount * sizeof(long));

    /* Check allocations */
    if (pageTable == NULL || memory == NULL || frameToPage == NULL || frameLastUsed == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(pageTable);
        free(memory);
        free(frameToPage);
        free(frameLastUsed);
        return 1;
    }

    /* Initialize page table: all entries invalid */
    for (i = 0; i < pageCount; i++) {
        pageTable[i].valid = 0;
        pageTable[i].frameNo = -1;
        pageTable[i].lastUsed = -1;
    }

    /* Initialize frame tracking: all frames empty */
    for (i = 0; i < frameCount; i++) {
        frameToPage[i] = -1;
        frameLastUsed[i] = -1;
    }

    /* Open input files */
    addressFile = fopen("addresses.txt", "r");
    if (addressFile == NULL) {
        fprintf(stderr, "Error: Could not open addresses.txt\n");
        free(pageTable);
        free(memory);
        free(frameToPage);
        free(frameLastUsed);
        return 1;
    }

    backingStore = fopen("backingstore.bin", "rb");
    if (backingStore == NULL) {
        fprintf(stderr, "Error: Could not open backingstore.bin\n");
        fclose(addressFile);
        free(pageTable);
        free(memory);
        free(frameToPage);
        free(frameLastUsed);
        return 1;
    }

    /* Process each logical address */
    while ((readStatus = readLogicalAddress(addressFile, &logicalAddress)) == READ_ADDRESS_OK) {
        accessCounter++;
        pageNo = getPageNo(logicalAddress, frameBits);
        offset = getOffset(logicalAddress, frameBits);

        /* Check if page table entry is valid (page hit) */
        if (pageTable[pageNo].valid) {
            /* Page table hit */
            frameNo = pageTable[pageNo].frameNo;
            physicalAddr = frameNo * pageSize + offset;
            data = memory[physicalAddr];

            /* Update LRU timestamp */
            pageTable[pageNo].lastUsed = accessCounter;
            frameLastUsed[frameNo] = accessCounter;

            /* Output: [Page Table] line */
            printf("[Page Table] (LA) %d -> (PA) %d: %d\n",
                   (int)logicalAddress, physicalAddr, (int)data);
        } else {
            /* Page fault */
            pageFaultCount++;

            freeFrame = findFreeFrame(frameToPage, frameCount);

            if (freeFrame != -1) {
                /* Free frame exists: allocate from low to high */
                frameNo = freeFrame;

                /* Load page from backing store */
                if (!loadPage(backingStore, memory, pageNo, frameNo, pageSize)) {
                    fclose(addressFile);
                    fclose(backingStore);
                    free(pageTable);
                    free(memory);
                    free(frameToPage);
                    free(frameLastUsed);
                    return 1;
                }

                /* Update page table */
                pageTable[pageNo].valid = 1;
                pageTable[pageNo].frameNo = frameNo;
                pageTable[pageNo].lastUsed = accessCounter;

                /* Update frame tracking */
                frameToPage[frameNo] = pageNo;
                frameLastUsed[frameNo] = accessCounter;

                /* Output: [Load Page] line */
                printf("    [Load Page] Page %d -> Frame%d\n", pageNo, frameNo);
            } else {
                /* No free frame: use LRU replacement */
                victimFrame = findLRUFrame(frameLastUsed, frameCount);
                oldPage = frameToPage[victimFrame];

                /* Invalidate old page table entry */
                pageTable[oldPage].valid = 0;
                pageTable[oldPage].frameNo = -1;
                pageTable[oldPage].lastUsed = -1;

                /* Load new page into victim frame */
                if (!loadPage(backingStore, memory, pageNo, victimFrame, pageSize)) {
                    fclose(addressFile);
                    fclose(backingStore);
                    free(pageTable);
                    free(memory);
                    free(frameToPage);
                    free(frameLastUsed);
                    return 1;
                }

                /* Update page table for new page */
                pageTable[pageNo].valid = 1;
                pageTable[pageNo].frameNo = victimFrame;
                pageTable[pageNo].lastUsed = accessCounter;

                /* Update frame tracking */
                frameToPage[victimFrame] = pageNo;
                frameLastUsed[victimFrame] = accessCounter;

                /* Output: [Replace page] line */
                printf("    [Replace page] Frame%d: Page %d -> Page %d\n",
                       victimFrame, oldPage, pageNo);
            }

            /* After page fault handled, compute physical address and output translation */
            frameNo = pageTable[pageNo].frameNo;
            physicalAddr = frameNo * pageSize + offset;
            data = memory[physicalAddr];

            /* Output: normal translation line */
            printf("(LA) %d -> (PA) %d: %d\n",
                   (int)logicalAddress, physicalAddr, (int)data);
        }
    }

    if (readStatus == READ_ADDRESS_INVALID) {
        fprintf(stderr, "Error: Invalid logical address in addresses.txt\n");
        fclose(addressFile);
        fclose(backingStore);
        free(pageTable);
        free(memory);
        free(frameToPage);
        free(frameLastUsed);
        return 1;
    }

    /* Close files */
    fclose(addressFile);
    fclose(backingStore);

    /* Generate stat.txt */
    writeStat(frameToPage, pageFaultCount, (int)accessCounter, frameCount);

    /* Free allocated memory */
    free(pageTable);
    free(memory);
    free(frameToPage);
    free(frameLastUsed);

    return 0;
}
