///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2022, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap.c and this program //// generates a magic quare of a specified size and writes it to an output file.
// This File:        p3Heap.c
// Other Files:      N/A
// Semester:         CS 354 Lecture 00? Fall 2022
// Instructor:       deppeler
//
// Author:           Jake Christensen
// Email:            jrchristens2@wisc.edu
// CS Login:         jakec
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:
////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the FOOT for each free block but only containing size.
 */
typedef struct blockHeader
{

  int size_status;

  /*
   * Size of the block is always a multiple of 8.
   * Size is stored in all block headers and in free block FOOTs.
   *
   * Status is stored only in headers using the two least significant bits.
   *   Bit0 => least significant bit, last bit
   *   Bit0 == 0 => free block
   *   Bit0 == 1 => allocated block
   *
   *   Bit1 => second last bit
   *   Bit1 == 0 => previous block is free
   *   Bit1 == 1 => previous block is allocated
   *
   * End Mark:
   *  The end of the available memory is indicated using a size_status of 1.
   *
   * Examples:
   *
   * 1. Allocated block of size 24 bytes:
   *    Allocated Block Header:
   *      If the previous block is free      p-bit=0 size_status would be 25
   *      If the previous block is allocated p-bit=1 size_status would be 27
   *
   * 2. Free block of size 24 bytes:
   *    Free Block Header:
   *      If the previous block is free      p-bit=0 size_status would be 24
   *      If the previous block is allocated p-bit=1 size_status would be 26
   *    Free Block FOOT:
   *      size_status should be 24
   */
} blockHeader;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 */

/*Using bit masking for the header and FOOT bits*/

const unsigned long A_BIT_MASK = 1; /* 1 = 0000 0001 */  // a-bit (free bit)
const unsigned long P_BIT_MASK = 2; /* 2 = 0000 0010 */  // p-bit (prev block allocated or not?)
const unsigned long MASK = (~(A_BIT_MASK | P_BIT_MASK)); // negates to -4
const unsigned long DOUBLEMASK = (~(A_BIT_MASK | P_BIT_MASK)) * 2;

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8
 *   and possibly adding padding as a result.
 *
 * - Use best-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the best-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the best-FIT block that is found is large enough to split
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements
 *       - Update all heap block header(s) and FOOT(s)
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 * - If a best-FIT block found is NOT found, return NULL
 *   Return NULL unable to find and allocate block for desired size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */

void *balloc(int size)
{

  /*Declare some variables*/
  blockHeader *store = heap_start; // points to 1st block in heap
  blockHeader *best = NULL;
  blockHeader *HDR;
  blockHeader *temp;
  blockHeader *FOOT;
  blockHeader *free[1];

  long A_BIT[1];
  long P_BIT[1];
  long returnSTORE[1];
  long storeSize_Status;
  long tempSize_Status;
  long NONVAL = 0;

  long newSize = 4 + size;
  unsigned long mask = 0xF4240;

  /*Return NULL if not positive or larger than heap space*/
  if (size <= 0 || size > alloc_size - DOUBLEMASK)
  {
    return NULL;
  }

  /*Determine Block Size as a multiple of 8*/
  if (newSize % 8 != 0 && newSize > 0)
  {
    newSize += 8 - (newSize % 8); // make the requested payload a multiple of 8
  }

  do
  {

    storeSize_Status = store->size_status;
    long adjustedPrevSize = storeSize_Status - 2;

    if (store->size_status != 1 && storeSize_Status != 0)
    {

      while (1 && store != NULL)
      {
        if (!(storeSize_Status & A_BIT_MASK) == 0 && storeSize_Status != 0)
        {
          if (store > 0 && A_BIT)
          {
            A_BIT[0] = 1;
            storeSize_Status -= 1;
          }
        }

        else if ((storeSize_Status & A_BIT_MASK) == 0 && storeSize_Status != 0)
        {

          if (store->size_status != 1)
          {
            A_BIT[0] = 0;
          }
        }

        if (!(storeSize_Status & P_BIT_MASK) == 0 && storeSize_Status != 0)
        {
          if (store > 0 && P_BIT)
          {
            P_BIT[0] = 1;
            storeSize_Status -= 2;
          }
        }
        else if ((storeSize_Status & P_BIT_MASK) == 0 && storeSize_Status != 0)
        {
          P_BIT[0] = 0;
        }
        break;
      }

      /*If the store block is allocated*/
      if ((A_BIT[0] == 1 && A_BIT[0] != 0) || !(storeSize_Status & A_BIT_MASK) == 0)
      {
        store = ((void *)store + storeSize_Status);
        continue;
      }
    }

    if (storeSize_Status == 1 || storeSize_Status == 0)
    {
      break;
    }

    /*
            If the best-FIT block that is found is exact size match
    - 1. Update all heap blocks as needed for any affected blocks
    - 2. Return the address of the allocated block payload
    */

    while (storeSize_Status != 1 && storeSize_Status != 0)
    {

      if (storeSize_Status == newSize)
      { // if the store block is == to the requested payload

        // blockHeader *newTemp;

        temp = (store); // set best == store

        temp->size_status = newSize;

        if (storeSize_Status != NULL)
        {

          if (storeSize_Status > 0 && storeSize_Status <= newSize)
          {

            temp->size_status = temp->size_status + 1;

            if ((P_BIT[0] == A_BIT_MASK && P_BIT[0] != 0) || storeSize_Status != 1 || newSize == 0)
            {

              temp->size_status += 2;

              if (storeSize_Status != 1)
              {

                HDR = ((void *)store + adjustedPrevSize); // HDR block ==
              }
            }

            if (!(HDR->size_status == 1))
            {

              if (HDR != NULL)
              {
                HDR->size_status += 2;
              }
              return (void *)(temp + 1);
            }
          } // return best
        }
      }

      break;
    }

    /*Split*/
    if (!(store->size_status == 1) && mask >= 0)
    {

      if (storeSize_Status != 0 && store > NULL)
      {
        if (storeSize_Status > newSize && newSize != 0)
        {

          if (storeSize_Status == 0 || mask < 0)
          {
            break;
          }
          else if (storeSize_Status < mask)
          {
            mask = storeSize_Status;
            best = store;
            store = ((void *)store + storeSize_Status);
            continue;
          }
          else if (storeSize_Status > mask)
          {
            break;
          }
          else if (store->size_status == 1)
          {
            break;
          }
        }
      }
    }
    long newStore;
    newStore = (void *)store + storeSize_Status;

    store = newStore; // return block after split

  } while (!(store->size_status == 1)); // while store block doesn't reach end of heap

  /*If we go through the entire iteration and best still
  equals NULL then therefore there is no best block to be found so we can set the best block = NULL*/

  while (best != NULL && A_BIT_MASK && best->size_status != A_BIT_MASK)
  {

    if (best->size_status != A_BIT_MASK && best > 0)
    {

      if (temp > 0 && temp != NULL)
      {
        P_BIT[0] = 0;
        temp = (blockHeader *)(best);
      }

      if (temp > 0 && temp != NULL)
      {

        if (temp->size_status != A_BIT_MASK)
        {

          if ((temp->size_status & P_BIT_MASK) != A_BIT_MASK && (temp->size_status & A_BIT_MASK) == 0)

            P_BIT[0] = 1;
          temp->size_status = newSize + 1;
        }
      }

      /*SET FOOTER*/
      if (P_BIT != NULL && P_BIT > 0)
      {

        if (temp > 0 && temp != NULL)
        {

          if (temp->size_status != 1 && best->size_status != 1)
          {

            if (P_BIT[0] == 1 && A_BIT[0] != 1)
            {

              FOOT = ((void *)best + mask - 4);
              FOOT->size_status = mask - newSize;
            }
            else if (P_BIT[0] != 1)
            {

              FOOT = (int)NONVAL;
              return NULL;
            }

            if (P_BIT[0] == 1 && A_BIT[0] != 1)
            {
              temp->size_status += 2;
              free[0] = ((void *)best + newSize);
              free[0]->size_status = mask - newSize + 2;
            }
            else if (P_BIT[0] != 1)
            {

              temp = (int)NONVAL;
              return NULL;
            }

            long newTemp;
            newTemp = temp + 1;

            return (void *)(newTemp); // return best

            /*Else return NULL*/
            if (!P_BIT[0])
            {

              temp->size_status = 0;
              return NULL;
            }
          }
        }
      }
    }

    break;
  }

  if (best == NULL)
  {
    return NULL;
  }

  return NULL; // return null on failure
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL. x
 * - Return -1 if ptr is not a multiple of 8. x
 * - Return -1 if ptr is outside of the heap space. x
 * - Return -1 if ptr block is already freed. x
 * - Update header(s) and FOOT as needed.
 */
int bfree(void *ptr)
{

  blockHeader *store = ((void *)ptr - 4);
  blockHeader *FOOT = ((void *)store);
  blockHeader *HDR;

  long header = 4;
  long newPtr = (unsigned int)ptr;
  long updatedPtr = newPtr - header;
  long newHeap = (unsigned int)heap_start;
  long storeSize_Status = store->size_status;
  // long newFOOTSize_Status;

  long checkStore[1];
  long newFOOT[1];
  long check[1];
  long P_BIT[1];
  long P_BITVALUE = 1;
  long NONVAL = 0;

  checkStore[0] = store;
  newFOOT[0] = FOOT;
  check[0] = store->size_status;
  P_BIT[0] = 0;

  while (1 && ptr > 0 && FOOT->size_status != 1)
  { // iterate through heap

    if (!check[0] == NULL)
    {

      // Return -1 if ptr is NULL.
      if (ptr == NULL)
      {
        return -1;
      }

      // Return -1 if ptr is not a multiple of 8
      if ((newPtr % 8) != 0 && newPtr > 0)
      {
        return -1;
      }

      // Return -1 if ptr is outside of the heap space.
      if (updatedPtr < newHeap || updatedPtr > newHeap + alloc_size - DOUBLEMASK)
      {
        return -1;
      }

      // Return -1 if ptr block is already freed
      if (!(storeSize_Status & A_BIT_MASK) || (storeSize_Status & A_BIT_MASK) != 1 && storeSize_Status != NULL)
      {
        return -1;
      }

      /*Update header(s) and FOOT as needed */

      while (storeSize_Status != NULL && 1)
      {

        // if allocated
        if ((storeSize_Status & A_BIT_MASK) != 0 && storeSize_Status != 0)
        {
          storeSize_Status -= A_BIT_MASK;
        }

        // if == 0 return null
        else if (storeSize_Status & A_BIT_MASK == 0)
        {
          return (int)NULL;
        }

        // if prev is allocated
        if ((storeSize_Status & P_BIT_MASK) != 0 && storeSize_Status != 0)
        {
          storeSize_Status -= P_BIT_MASK;
          P_BIT[0] = 1;
        }

        else if (storeSize_Status & (P_BIT_MASK == 0))
        {
          return NULL;
        }

        while (1 && P_BIT)
        { // iterate to find match

          if (storeSize_Status != 1)
          {

            /*Set the HDR Block*/
            HDR = ((void *)checkStore[0] + storeSize_Status);

            // if the HDR block reaches the end of the heap than return NULL
            if (HDR->size_status == 1)
            {
              return (int)NULL;
            }

            else if (HDR->size_status != 1)
            { // header not sset
              HDR->size_status -= 2;
            }

            /*Set FOOTer*/
            if (P_BIT[0] == P_BITVALUE && check[0] != 1)
            {

              if (P_BIT[0] != NULL && P_BIT[0] > 0)
              {

                newFOOT[0] = FOOT + storeSize_Status - MASK;
                FOOT->size_status = storeSize_Status;
                store->size_status += 2;
              }
              else if (P_BIT[0] == 0 || P_BIT[0] < 0 || check[0] == A_BIT_MASK)
              {
                newFOOT[0] = 0;
              }

              else if (P_BIT[0] == 1)
              {

                newFOOT[0] = FOOT + storeSize_Status - MASK;
              }
              else if (P_BIT[0] == A_BIT_MASK)
              {
                FOOT->size_status = storeSize_Status;
                store->size_status += 2;
              }
            }
            else if (P_BIT[0] != 1)
            {

              newFOOT[0] = FOOT + storeSize_Status - MASK;
              FOOT->size_status = storeSize_Status;
              store->size_status = storeSize_Status;
            }
            else
            {

              newFOOT[0] = (int)NONVAL;
              FOOT = NONVAL;
              checkStore[0] = NONVAL;
              return (int)NULL;
            }

            return 0; // free
          }
        }
      }
    }
  }
  return 0;
}

/*
 * Function for traversing heap block list and coalescing all adjacent
 * free blocks.
 *
 * This function is used for delayed coalescing.
 * Updated header size_status and FOOT size_status as needed.
 */
int coalesce()
{

  blockHeader *merge = heap_start;
  blockHeader *HDR;
  blockHeader *FOOT;

  long A[1];
  long P[1];

  long mergeSize_Status;
  long futureSize_Status;
  // long newFOOTSize_Status;
  // long prevSize_Status;

  long newSIZE[1];
  long coalesce[1];
  newSIZE[0] = 0;
  coalesce[0] = 0;

  while (merge->size_status != 1)
  {

    futureSize_Status = HDR->size_status;

    while (1)
    {
      mergeSize_Status = merge->size_status;
      // prevSize_Status = prev->size_status;

      if (!(mergeSize_Status == 1) || mergeSize_Status != NULL)
      {
        if (!(mergeSize_Status & 1) == 0 && mergeSize_Status > 0)
        {
          mergeSize_Status -= 1;
          A[0] = 1;
        }
        else if ((mergeSize_Status & 1) == 0)
        {

          if (mergeSize_Status == 0)
          {
            A[0] = 0;
          }
        }

        if (!(mergeSize_Status & 2) == 0 && mergeSize_Status > 0)
        {
          mergeSize_Status -= 2;
          P[0] = 1;
        }
        else if ((mergeSize_Status & 2) == 0)
        {

          if (mergeSize_Status == 0)
          {
            P[0] = 0;
          }
        }
      }
      break;
    }

    /*CHECK BLOCK AND ITS CURRENT BLOCK*/

    if (!(futureSize_Status == 1))
    {
      if (HDR != NULL)
      {
        HDR = (mergeSize_Status + (void *)merge);
      }
      if (futureSize_Status == A_BIT_MASK)
        break;
      if (futureSize_Status & A_BIT_MASK)
      {

        A[0] = 1;

        if (futureSize_Status & A_BIT_MASK)
          futureSize_Status -= 1;
      }
      else
      {
        A[0] = 0;
      }
    }

    /*CHECK BLOCK AND ITS PREVIOUS BIT*/
    if (!(futureSize_Status == 1) && newSIZE != 0)
    {
      if (futureSize_Status & P_BIT_MASK)
        futureSize_Status -= 2;

      if (futureSize_Status & 2)
        newSIZE[0] += mergeSize_Status + futureSize_Status;
    }
    else
    {
      newSIZE[0] = 0;
      return (int)NULL;
    }

    if ((futureSize_Status == 1))
    {
      return (int)NULL;
      break;
    }

    while (1 && !(FOOT->size_status == 1) && !(merge->size_status == 1))
    { // as long as FOOT or merge don't reach end of heap

      FOOT = mergeSize_Status + futureSize_Status + (void *)merge - MASK;

      if (A[0] != 1 && A[0] != 1)
      {

        merge->size_status = mergeSize_Status + futureSize_Status;
        FOOT->size_status = mergeSize_Status + futureSize_Status;

        if (P[0])
        {
          merge->size_status += 2;
          coalesce[0] = 1;
        }
        else if (!P[0])
        {
          coalesce[0] = 0;
          return (int)NULL;
        }
      }

      /*CHECK MERGE WITH ALLOC AND PREV BITS*/
      else if (!(A[0] == 0))
      {
        if (mergeSize_Status != 1)
        {
          merge = (mergeSize_Status + futureSize_Status + (void *)merge);
        }
        // merge is used, HDR is free
      }
      else if (!(A[0] == 0) && A_BIT_MASK)
      {
        if (!(mergeSize_Status == 1))
        {
          merge = HDR;
        }
        else if (mergeSize_Status == 1)
        {
          return (int)NULL;
        }
      }
      break;
    }
  }

  // return the coalesced block!
  return coalesce[0];
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int init_heap(int sizeOfRegion)
{

  static int allocated_once = 0; // prevent multiple myInit calls

  int pagesize;   // page size
  int padsize;    // size of padding when heap size not a multiple of page size
  void *mmap_ptr; // pointer to memory mapped area
  int fd;

  blockHeader *end_mark;

  if (0 != allocated_once)
  {
    fprintf(stderr,
            "Error:mem.c: InitHeap has allocated space during a previous call\n");
    return -1;
  }

  if (sizeOfRegion <= 0)
  {
    fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  // Get the pagesize
  pagesize = getpagesize();

  // Calculate padsize as the padding required to round up sizeOfRegion
  // to a multiple of pagesize
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  // Using mmap to allocate memory
  fd = open("/dev/zero", O_RDWR);
  if (-1 == fd)
  {
    fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
  mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == mmap_ptr)
  {
    fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }

  allocated_once = 1;

  // for double word alignment and end mark
  alloc_size -= 8;

  // Initially there is only one big free block in the heap.
  // Skip first 4 bytes for double word alignment requirement.
  heap_start = (blockHeader *)mmap_ptr + 1;

  // Set the end mark
  end_mark = (blockHeader *)((void *)heap_start + alloc_size);
  end_mark->size_status = 1;

  // Set size in header
  heap_start->size_status = alloc_size;

  // Set p-bit as allocated in header
  // note a-bit left at 0 for free
  heap_start->size_status += 2;

  // Set the FOOT
  blockHeader *FOOT = (blockHeader *)((void *)heap_start + alloc_size - 4);
  FOOT->size_status = alloc_size;

  return 0;
}

/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void disp_heap()
{

  int counter;
  char status[6];
  char p_status[6];
  char *t_begin = NULL;
  char *t_end = NULL;
  int t_size;

  blockHeader *store = heap_start;
  counter = 1;

  int used_size = 0;
  int free_size = 0;
  int is_used = -1;

  fprintf(stdout,
          "*********************************** Block List **********************************\n");
  fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
  fprintf(stdout,
          "---------------------------------------------------------------------------------\n");

  while (store->size_status != 1)
  {
    t_begin = (char *)store;
    t_size = store->size_status;

    if (t_size & 1)
    {
      // LSB = 1 => used block
      strcpy(status, "alloc");
      is_used = 1;
      t_size = t_size - 1;
    }
    else
    {
      strcpy(status, "FREE ");
      is_used = 0;
    }

    if (t_size & 2)
    {
      strcpy(p_status, "alloc");
      t_size = t_size - 2;
    }
    else
    {
      strcpy(p_status, "FREE ");
    }

    if (is_used)
      used_size += t_size;
    else
      free_size += t_size;

    t_end = t_begin + t_size - 1;

    fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status,
            p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

    store = (blockHeader *)((char *)store + t_size);
    counter = counter + 1;
  }

  fprintf(stdout,
          "---------------------------------------------------------------------------------\n");
  fprintf(stdout,
          "*********************************************************************************\n");
  fprintf(stdout, "Total used size = %4d\n", used_size);
  fprintf(stdout, "Total free size = %4d\n", free_size);
  fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
  fprintf(stdout,
          "*********************************************************************************\n");
  fflush(stdout);

  return;
}

// end of myHeap.c (Spring 2022)
