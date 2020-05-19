/* 
 * File:   tableDB.h
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 *
 * Created on 11 de julio de 2018, 22:14
 */

#ifndef TABLEDB_H
#define	TABLEDB_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C"
{
#endif

// Size of a single table entry in bytes    
#define TC_ENTRY_SIZE 128
#define TC_TEXT_SIZE ((TC_ENTRY_SIZE)-2*sizeof(uint64_t))

/** Structure for a single table entry
 */
typedef struct
{
    uint64_t id;
    uint64_t used;
    char text[TC_TEXT_SIZE];
} TC_tableEntry_t;

int TC_openTable();
int TC_flushAllBlocks();
int TC_writeEntryAsync(int fileIndex, TC_tableEntry_t * entry);
int TC_writeEntrySync(int fileIndex, TC_tableEntry_t * entry);
int TC_readEntry(int fileIndex, TC_tableEntry_t *entry);
int TC_closeTable();

#ifdef	__cplusplus
}
#endif

#endif	/* TABLEDB_H */

