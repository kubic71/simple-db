/* 
 * File:   tableCache.c
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 *
 * Created on 11 de julio de 2018, 22:10
 */

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "tableCache.h"

/** This is the variable containing the cache
 */
static TC_cache_t *cache;
static int fileDescriptor;

/**
 * Translate the index of an entry into the file to the index of the block
 */
#define fileIndex2blockIndex(fileIndex) ((fileIndex)/(TC_BLOCK_ENTRIES))
/**
 * Offset of an entry inside a block
 */
#define fileIndex2blockOffset(fileIndex) ((fileIndex)%(TC_BLOCK_ENTRIES))

/**
 * Initialize the file cache
 * @param cache pointer to current cache structure
 */
void initCache(TC_cache_t *cache)
{
    int i;
    for (i = 0; i < TC_CACHE_BLOCKS; i++)
    {
        cache->controlBlocks[i].used = 0;
        cache->controlBlocks[i].dirty = 0;
        cache->controlBlocks[i].ramIndex = -1;
        cache->controlBlocks[i].fileIndex = -1;
    }
}

/**
 * Flush the block indicated by cache position onto file
 */
int flushBlock(TC_cache_t *cache, int cacheIndex)
{
    off_t offset;
    int status;

    if (cache->controlBlocks[cacheIndex].used)
    {
        if (cache->controlBlocks[cacheIndex].dirty)
        {
            offset = lseek(fileDescriptor, cache->controlBlocks[cacheIndex].fileIndex*TC_CACHE_BLOCK_SIZE, SEEK_SET);
            if (offset == (off_t) - 1) return -1;
            status = write(fileDescriptor, &(cache->dataBlocks[cacheIndex]), TC_CACHE_BLOCK_SIZE);
            if (status == -1) return -1;
            printf("Cache block %d flushed to file block %d\n", cacheIndex, cache->controlBlocks[cacheIndex].fileIndex);
            cache->controlBlocks[cacheIndex].dirty = 0;
            return 0;
        }
    }
    else return -1;
}

/**
 * Read file block into cache block indicated by cache position
 */
int readBlock(TC_cache_t *cache, int fileBlock, int cacheIndex)
{
    off_t offset;
    int status;

    if (cache->controlBlocks[cacheIndex].dirty)
    {
        if (flushBlock(cache, cacheIndex) == -1) return -1;
    }
    else
    {
        cache->controlBlocks[cacheIndex].used = 1;
        cache->controlBlocks[cacheIndex].dirty = 0;
        cache->controlBlocks[cacheIndex].fileIndex = fileBlock;
        // Seek to file position
        offset = lseek(fileDescriptor, fileBlock*TC_CACHE_BLOCK_SIZE, SEEK_SET);
        if (offset == (off_t) - 1) return -1;
        // Read data
        status = read(fileDescriptor, &(cache->dataBlocks[cacheIndex]), TC_CACHE_BLOCK_SIZE);
        // If EOF, fill buffer with zeroes
        if (status < TC_CACHE_BLOCK_SIZE)
        {
            memset((void *) &(cache->dataBlocks[cacheIndex]), 0, TC_CACHE_BLOCK_SIZE);
        }
        // Else check if error
        if (status == -1) return -1;
        cache->controlBlocks[cacheIndex].dirty = 0;
        return 0;
    }
}

/**
 * Find file block in cache
 * @param cache
 * @param fileBlock
 * @return -1 if not present
 */
int findBlockInCache(TC_cache_t *cache, int fileBlock)
{
    int i;
    for (i = 0; i < TC_CACHE_BLOCKS; i++)
    {
        if (cache->controlBlocks[i].used && cache->controlBlocks[i].fileIndex == fileBlock)
            return i;
    }
    return -1;
}

/**
 * Find free block in cache
 * @param cache
 * @return Index of block
 */
int findFreeBlockInCache(TC_cache_t *cache)
{
    static int lastdirtystart = 0;

    int i;
    int found = -1;

    // Return any unused block
    for (i = 0; i < TC_CACHE_BLOCKS; i++)
    {
        if (!cache->controlBlocks[i].used)
        {
            printf("Empty cache block %d assigned\n", i);
            found = i;
            break;
        }
    }

    // If not, return any clean block
    if (found == -1)
    {
        int j;
        for (j = lastdirtystart; j < TC_CACHE_BLOCKS + lastdirtystart; j++)
        {
            i = j % TC_CACHE_BLOCKS;
            if (cache->controlBlocks[i].used && !cache->controlBlocks[i].dirty)
            {
                printf("Clean cache block %d (file block %i) replaced\n", i, cache->controlBlocks[i].fileIndex);
                found = i;
                lastdirtystart = (lastdirtystart + 1) % TC_CACHE_BLOCKS;
                break;
            }
        }
    }

    // If not, clean any dirty block and return it
    if (found == -1)
    {
        int j;
        for (j = lastdirtystart; j < TC_CACHE_BLOCKS + lastdirtystart; j++)
        {
            i = j % TC_CACHE_BLOCKS;
            if (cache->controlBlocks[i].used && cache->controlBlocks[i].dirty)
            {
                printf("Dirty cache block %d (file block %i) replaced\n", i, cache->controlBlocks[i].fileIndex);
                flushBlock(cache, i);
                found = i;
                break;
            }
        }
        lastdirtystart = (lastdirtystart + 1) % TC_CACHE_BLOCKS;
    }
    cache->controlBlocks[found].used = 1;
    cache->controlBlocks[found].fileIndex = -1;
    return found;
}

/**
 * Write an entry to the file asynchronously. It copies the entry to the cache and marks the entry as dirty.
 * @param fileIndex Index of entry into file.
 * @param entry Pointer to entry.
 * @return -1=error
 */
int TC_writeEntryAsync(int fileIndex, TC_tableEntry_t * entry)
{
    int cacheIndex;
    int fileBlock = fileIndex2blockIndex(fileIndex);
    // Find block in cache
    cacheIndex = findBlockInCache(cache, fileBlock);
    // If not found, read it onto a cache block
    if (cacheIndex == -1)
    {
        cacheIndex = findFreeBlockInCache(cache);
        cache->controlBlocks[cacheIndex].fileIndex = fileBlock;
        if (readBlock(cache, fileBlock, cacheIndex) == -1) return -1;
    }
    // Copy entry to cache
    memcpy(&cache->dataBlocks[cacheIndex].entries[fileIndex2blockOffset(fileIndex)], entry, TC_ENTRY_SIZE);
    // Mark block as dirty
    cache->controlBlocks[cacheIndex].dirty = 1;
    return 0;
}

/**
 * Write an entry to the file asynchronously. It copies the entry to the cache and writes the entry.
 * @param fileIndex Index of entry into file.
 * @param entry Pointer to entry.
 * @return -1=error
 */
int TC_writeEntrySync(int fileIndex, TC_tableEntry_t * entry)
{
    int cacheIndex;
    int fileBlock = fileIndex2blockIndex(fileIndex);
    // Find block in cache
    cacheIndex = findBlockInCache(cache, fileBlock);
    // If not found, read it onto a cache block
    if (cacheIndex == -1)
    {
        cacheIndex = findFreeBlockInCache(cache);
        cache->controlBlocks[cacheIndex].fileIndex = fileBlock;
        if (readBlock(cache, fileBlock, cacheIndex) == -1) return -1;
    }
    // Copy entry to cache
    memcpy(entry, (const void *) &cache->dataBlocks[cacheIndex].entries[fileIndex2blockOffset(fileIndex)], TC_ENTRY_SIZE);
    // Mark block as dirty
    cache->controlBlocks[cacheIndex].dirty = 1;
    // Flush the block
    flushBlock(cache, cacheIndex);
    return 0;
}

/**
 * Read an entry from the file. It reads the cache block if neede and copies the entry from the cache.
 * @param fileIndex Index of entry into file.
 * @param entry Pointer to entry.
 * @return -1=error
 */
int TC_readEntry(int fileIndex, TC_tableEntry_t *entry)
{
    int cacheIndex;
    int fileBlock = fileIndex2blockIndex(fileIndex);
    // Find block in cache
    cacheIndex = findBlockInCache(cache, fileBlock);
    // If not found, read it onto a cache block
    if (cacheIndex == -1)
    {
        cacheIndex = findFreeBlockInCache(cache);
        cache->controlBlocks[cacheIndex].fileIndex = fileBlock;
        if (readBlock(cache, fileBlock, cacheIndex) == -1) return -1;
    }
    // Copy entry from cache
    memcpy(entry, (const void *) &cache->dataBlocks[cacheIndex].entries[fileIndex2blockOffset(fileIndex)], TC_ENTRY_SIZE);
    return 0;
}

/**
 * Initialize cache for table
 * @return 
 */
int TC_openTable()
{
    fileDescriptor = -1;
    cache = calloc(1, sizeof (TC_cache_t));
    initCache(cache);
    fileDescriptor = open(TC_FILENAME, O_SYNC | O_RDWR | O_CREAT, S_IRWXU);
    if (fileDescriptor == -1)
    {
        perror("Error opening file\n");
        return -1;
    }
    return 0;
}

/**
 * Close and remove cache for table
 * @return 
 */
int TC_closeTable()
{
    TC_flushAllBlocks();
    free(cache);
    close(fileDescriptor);
    return 0;
}

/**
 * Flush all blocks of the file cache
 * @param 
 * @return 
 */
int TC_flushAllBlocks()
{
    int i;
    for (i = 0; i < TC_CACHE_BLOCKS; i++)
    {
        if (flushBlock(cache, i) == -1) return -1;
    }
}

#ifdef	__cplusplus
}
#endif
