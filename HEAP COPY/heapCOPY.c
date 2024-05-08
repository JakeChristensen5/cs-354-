
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "myHeap.h"

typedef struct blockHeader {

    int size_status;
    
} blockHeader;


blockHeader *heapStart = NULL;

int allocsize;


void* myAlloc(int size) {
    if(size <= 0) {
	fprintf(stderr, "Error:myHeap.c: Requested size is not positive\n");
	return NULL;
    }
    int sizeNeeded = 4 + size;
    while(sizeNeeded % 8 != 0)
	 sizeNeeded++;
    blockHeader *current = heapStart;
    blockHeader *newAlloc;
    blockHeader *newFree;
    blockHeader *currBest = NULL;
    blockHeader *nextBLK;
    int t_size;
    int currBestSize = 1000000;
    int is_used   = -1;
    int p_is_used = -1;
    while (current->size_status != 1) {
	//printf("%08x\n", (unsigned int)current);
        t_size = current->size_status;

	if (t_size & 1) {
            // LSB = 1 => used block
            is_used = 1;
            t_size = t_size - 1;
        } else {
            is_used = 0;
        }

        if (t_size & 2) {
            t_size = t_size - 2;
	    p_is_used = 1;
        } else {
	    p_is_used = 0;
        }

        // is_used == true, then go to the next header; current + t_size
        if (is_used) {
            current = (blockHeader*)((char*)current + t_size);
            continue;
        }

	// split
	if(t_size > sizeNeeded) {
	    if(t_size < currBestSize) {
		currBestSize = t_size;
		currBest = current;
		current = (blockHeader*)((char*)current + t_size);
		//printf("1:%08x\n", (unsigned int)current);
		continue;
	    }
	}
	// exact match
	if(t_size == sizeNeeded) {
	    newAlloc = (blockHeader*)(current);
	    newAlloc->size_status = sizeNeeded;
            newAlloc->size_status += 1;
	    if(p_is_used)
		newAlloc->size_status += 2;
	    nextBLK = (blockHeader*)((char*)current + t_size);
	    if(nextBLK->size_status != 1)
        	nextBLK->size_status += 2;
	    return (void*)(newAlloc + 1);
	}
	current = (blockHeader*)((char*)current + t_size);
	//printf("2:%08x\n", (unsigned int)current);
    }
    if(currBest != NULL) {
        newAlloc = (blockHeader*)(currBest);
	p_is_used = 0;
	if(newAlloc->size_status & 2)
	    p_is_used = 1;
	// account for allocation bit (a-bit)
        newAlloc->size_status = sizeNeeded + 1;
        if(p_is_used)
            newAlloc->size_status += 2;
        newFree = (blockHeader*)((char*)currBest + sizeNeeded);
        //printf("%08x", (unsigned int)(current));
        //printf("%08x", (unsigned int)(current + sizeNeeded));
	// account for the previous bit (p-bit)
        newFree->size_status = currBestSize - sizeNeeded + 2;
        // Set the footer
	// subtract by 4 to align within block of memory
        blockHeader *footer = (blockHeader*) ((char*)currBest + currBestSize - 4);
        footer->size_status = currBestSize - sizeNeeded;
	// add one to return the address of the payload not the header (4 bytes)
        return (void*)(newAlloc + 1);
    }
    return NULL;
}


int myFree(void *ptr) {
    //printf("ptr: %08x\n", (unsigned int)ptr);
    if(ptr == NULL) {
	fprintf(stderr, "Error:myHeap.c: Ptr is NULL.\n");
        return -1;
    }
    if(((int)ptr % 8) != 0) {
        fprintf(stderr, "Error:myHeap.c: Ptr is not a multiple of 8.\n");
        return -1;
    }
    //printf("ptr - 4: %08x\n", (unsigned int)ptr - 4);
    //printf("heapStart: %08x\n", (unsigned int)heapStart);
    //printf("heapStart + allocsize - 8: %08x\n", (unsigned int)heapStart + allocsize - 8);
    if((unsigned int)ptr - 4 < (unsigned int)heapStart ||
	(unsigned int)ptr - 4 > (unsigned int)heapStart + allocsize - 8) {
        fprintf(stderr, "Error:myHeap.c: Ptr is outside of the heap space.\n");
        return -1;
    }

    //blockHeader* current = (blockHeader*)((void*)ptr);
    //printf("current: %08x\n", (unsigned int)current);
    blockHeader* current = (blockHeader*)((char*)ptr - 4);
    //printf("current - 4: %08x\n", (unsigned int)current);
    //current = (blockHeader*)((void*)ptr - 1);
    //printf("current - 1: %08x\n", (unsigned int)current);
    int size = current->size_status;
    if(!(size & 1)) {
        fprintf(stderr, "Error:myHeap.c: Ptr is already freed.\n");
        return -1;
    }
    // get the size and keep track of previous allocation so that we can
    // account for it when we update the header for current
    int p_alloc = 0;
    // & with one to check the least significant bit (true if odd, false if even)
    if (size & 1)
        size = size - 1;
    // & with two to check the second least significant bit (true if the bit is 1,
    // false if the bit is 0)
    if (size & 2) {
        size = size - 2;
	p_alloc = 1;
    }
    // set the footer
    if(p_alloc) {
	blockHeader *footer = (blockHeader*) ((char*)current + size - 4);
	footer->size_status = size;
	current->size_status = size + 2;
    } else {
	blockHeader *footer = (blockHeader*) ((char*)current + size - 4);
	footer->size_status = size;
	current->size_status = size;
    }

    // go to the next block and set the p-bit to 0
    blockHeader* nextBLKH = (blockHeader*)((char*)current + size);
    // check if the next block is the end mark
    if(nextBLKH->size_status != 1)
	nextBLKH->size_status -= 2;

    return 0;
}

/*
 * Function for traversing heap block list and coalescing all adjacent
 * free blocks.
 *
 * This function is used for delayed coalescing.
 * Updated header size_status and footer size_status as needed.
 */
int coalesce() {
    blockHeader* current = (blockHeader*)heapStart;
    int currSize = 0;
    blockHeader* next;
    int nextSize = 0;
    int currIsUsed = -1;
    int nextIsUsed = -1;
    blockHeader* newFooter;
    int currPBit = 0;
    int merged = 0;
    int totalSize = 0;
    while (current->size_status != 1) {
	currSize = current->size_status;

        if (currSize & 1) {
            currIsUsed = 1;
            currSize -= 1;
        } else
            currIsUsed = 0;

        if (currSize & 2) {
            currSize -= 2;
	    currPBit = 1;
	} else
	    currPBit = 0;

	next = (blockHeader*)((char*)current + currSize);
	nextSize = next->size_status;
	if(nextSize == 1)
	    break;
	if (nextSize & 1) {
	    nextIsUsed = 1;
	    nextSize -= 1;
	} else
	    nextIsUsed = 0;

	if (nextSize & 2)
	    nextSize -= 2;
	totalSize += currSize + nextSize;
	//printf("Current %08x\n", (unsigned int)current);
	//printf("Next %08x\n", (unsigned int)next);
        // both current and next are free
	if (!currIsUsed && !nextIsUsed) {
            current->size_status = currSize + nextSize;
	    newFooter = (blockHeader*)((char*)current + currSize + nextSize - 4);
	    newFooter->size_status = currSize + nextSize;
	    if(currPBit)
		current->size_status += 2;
	    merged = 1;
	}
	// current is free, next is used
	else if (nextIsUsed)
	    current = (blockHeader*)((char*)current + currSize + nextSize);
	// current is used, next is free
	else if (currIsUsed && !nextIsUsed)
	    current = next;

    }

	return merged;
}


