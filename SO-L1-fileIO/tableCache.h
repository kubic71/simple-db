/* 
 * File:   tableCache.h
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 *
 * Created on 11 de julio de 2018, 16:52
 */

#ifndef TABLECACHE_H
#define	TABLECACHE_H

#include "parameters.h"

#ifdef	__cplusplus
extern "C"
{
#endif


//#pragma pack(1)

/** Structure for a cache block made of TC_BLOCK_ENTRIES table entries
 */
typedef struct
{
    TC_tableEntry_t entries[TC_BLOCK_ENTRIES];
} TC_cacheBlock_t;

/** Control structure for a single cache block
 */
typedef struct
{
    // Is block used?
    int used;
    // Is block dirty?
    int dirty;
    // Index of block on memory cache (in blocks)
    int ramIndex;
    // Index of block on file (in blocks)
    int fileIndex;
} TC_cacheBlockCntl_t;

/** The cache of the table
 */
typedef struct
{
    TC_cacheBlockCntl_t controlBlocks[TC_CACHE_BLOCKS];
    TC_cacheBlock_t dataBlocks[TC_CACHE_BLOCKS];
} TC_cache_t;

#ifdef	__cplusplus
}
#endif

#endif	/* TABLECACHE_H */

