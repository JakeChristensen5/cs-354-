////////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// Purpose: A cache simulator program.
// This File:        csim.c
// Other Files:      N/A
// Semester:         CS 354 Lecture 001 Fall 2022
// Instructor:       deppeler
//
// Author:           Jake Christensen
// Email:            Jrchristens2@wisc.edu
// CS Login:         jakec
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013,2019-2020
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Fall 2022
//
////////////////////////////////////////////////////////////////////////////////

/*
 * csim.c:
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics for the number of hits, misses, and evictions.
 * The replacement policy is LRUCOUNTER.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
/*Defining Global Constants*/
#define powerOFSETS(S) pow(2, s)  // S = 2^s
#define powerOFBYTES(B) pow(2, b) // B = 2^b

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLES **********************************************/

// Globals set by command line args.
int b = 0; // number of block (b) bits
int s = 0; // number of set (s) bits
int E = 0; // number of lines per set

// Globals derived from command line args.
int B; // block size in bytes: B = 2^b
int S; // number of sets: S = 2^s

// Global counters to LRUCOUNTER cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

// Global to control trace output
int verbosity = 0; // print trace if set
/******************************************************************************/

int lruCount = 1; /*Counter for the LRU policy*/

// Type mem_addr_t: Use when dealing with addresses or address SETMASKs.
typedef unsigned long long int mem_addr_t;

// Type cache_line_t: Use when dealing with cache lines.
// TODO - COMPLETE THIS TYPE
typedef struct cache_line
{
    char valid;
    mem_addr_t cacheTAG;
    long long LRUCOUNTER; // counter for the placement policy
    // Add a data member as needed by your implementation for LRUCOUNTER LRUCOUNTERing.
} cache_line_t;

// Type cache_set_t: Use when dealing with cache sets
// Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t *cache_set_t;

// Type cache_t: Use when dealing with the cache.
// Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t *cache_t;

// Create the cache we're simulating.
// Note: A cache is a pointer to a heap array of one or more cache sets.
cache_t cache;

/* TODO - COMPLETE THIS FUNCTION
 * init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and cacheTAGs with 0s.
 */
void init_cache()
{

    /*Since cache = ptr to heap array we can dynamically
    allocate memory for it*/

    // Allocate for each S (set)
    cache = malloc(sizeof(cache_set_t) * powerOFSETS(S));

    /*Checking the return value of malloc()*/
    if (cache == NULL)
    {
        printf("Memory was never allocated.\n");
        exit(1); // exit the program
    }
    else
    {

        if (cache != NULL)
        {
            int i = 0;
            while (i < powerOFSETS(S))
            {

                /*Allocate memory for the bytes of the cache block*/
                cache[i] = (cache_line_t *)malloc(sizeof(cache_line_t) * powerOFBYTES(B));

                int j = 0;
                do
                {
                    cache[i][j].cacheTAG = 0; // init cacheTAG with 0
                    cache[i][j].valid = 0;    // init valid with 0

                    cache[i][j].LRUCOUNTER = 0; // set lru to 0
                    j += 1;

                } while (j < E);

                i += 1;
            }
        }
    }
}

/* TODO - COMPLETE THIS FUNCTION
 * free_cache:
 * Frees all heap allocated memory used by the cache.
 */
void free_cache()
{

    int i = 0;

    if (cache != NULL)
    {
        do
        {
            free(cache[i]); // free each element within the array of the cache
            i += 1;
        } while (i < S);
    }

    free(cache);  // free the cache
    cache = NULL; // set cache = NULL to avoid mem leaks
}

/* TODO - COMPLETE THIS FUNCTION
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_cnt
 * If not in cache, cache it (set cacheTAG), increment miss_cnt
 * If a line is evicted, increment evict_cnt
 */
void access_data(mem_addr_t addr)
{

    /*Declare some local variables*/
    long SETMASK;
    long next = 0;
    long addBits = s + b;
    long valid = 1;

    /*Set and cacheTAG*/
    mem_addr_t cacheTAG = addr >> (addBits);
    SETMASK = (addr - (cacheTAG << (addBits)));
    mem_addr_t cacheSET = SETMASK >> b;
    /***********************************/

    /*Cache is empty then exit program*/
    if (cache == 0 || cache == NULL)
    {
        printf("The Cache is Empty!\n");
        exit(1);
    }

    if ((cache + cacheSET) == 0)
    {
        printf("ERROR!\n");
        exit(1);
    }

    if ((cache + cacheTAG) == 0)
    {
        printf("ERROR!\n");
        exit(1);
    }

    while (1 && cache != NULL)
    {

        /*If the memory address is already found in the cache; mark it as a hit and i++ hit*/
        long i = 0;
        long j = 0;
        long k = 0;

        if (E != 0)
        {
            while (i < E)
            { // i < lines

                /*If the cache cacheTAG = correct cacheTAG && cache cacheTAG = valid cacheTAG*/
                if (cache[cacheSET][i].cacheTAG == cacheTAG)
                {
                    if (cache[cacheSET][i].valid == valid)
                    {
                        if (cache[cacheSET][i].valid != 0)
                        {
                            hit_cnt += 1; // incr hit count
                            cache[cacheSET][i].LRUCOUNTER = lruCount;

                            lruCount += 1; // incr lru counter
                            return;
                        }
                    }
                }
                i += 1;
            }
        }

        miss_cnt += 1; // if here then add a miss

        if (E != 0)
        {
            while (j < E)
            {
                if (cache[cacheSET][j].valid != valid)
                {
                    if (cache != NULL)
                    {
                        cache[cacheSET][j].valid = valid;
                        cache[cacheSET][j].LRUCOUNTER = lruCount;
                        cache[cacheSET][j].cacheTAG = cacheTAG;

                        lruCount += 1;
                        return;
                    }
                }
                j += 1;
            }
        }

        do
        {
            if (cache != 0)
            {

                if (E != 0)
                {

                    if (cache[cacheSET][next].LRUCOUNTER > cache[cacheSET][k].LRUCOUNTER)
                    {
                        next = k;
                    }
                }

                else
                {
                }
            }

            k += 1;
        } while (k < E);

        break;
    }

    /*Line is evicted if reached here*/
    /*Set cacheTAG, VALID, and COUNTER*/

    while (1 && cache != NULL)
    {

        if (next != 0)
            if (lruCount > 1)
                cache[cacheSET][next].valid = valid;
        cache[cacheSET][next].cacheTAG = cacheTAG;   // Set cacheTAG
        cache[cacheSET][next].LRUCOUNTER = lruCount; // Set LRU counter

        lruCount += 1;  // increment counter
        evict_cnt += 1; // increment eviction

        break;
    }
    return;
}
/* TODO - FILL IN THE MISSING CODE
 * replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses
 */
void replay_trace(char *trace_fn)
{
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE *trace_fp = fopen(trace_fn, "r");

    if (!trace_fp)
    {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL)
    {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M')
        {
            sscanf(buf + 3, "%llx,%u", &addr, &len);
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access

            if (buf[1] == 'L')
            {
                access_data(addr);
            }
            else if (buf[1] == 'S')
            {
                access_data(addr);
            }

            else if (buf[1] == 'M')
            {
                // access twice
                access_data(addr);
                access_data(addr);
            }
            else
            {
                NULL;
            }

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}
/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */
void print_usage(char *argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for block offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */
void print_summary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE *output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main:
 * Main parses command line args, makes the cache, replays the memory accesses
 * free the cache and print the summary statistics.
 */
int main(int argc, char *argv[])
{
    char *trace_file = NULL;
    char c;

    // Parse the command line arguments: -h, -v, -s, -E, -b, -t
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1)
    {
        switch (c)
        {
        case 'b':
            b = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'h':
            print_usage(argv);
            exit(0);
        case 's':
            s = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        default:
            print_usage(argv);
            exit(1);
        }
    }

    // Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL)
    {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    // Initialize cache.
    init_cache();

    // Replay the memory access trace.
    replay_trace(trace_file);

    // Free memory allocated for cache.
    free_cache();

    // Print the statistics to a file.
    // DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}

// 202209
