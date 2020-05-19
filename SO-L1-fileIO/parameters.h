/* 
 * File:   parameters.h
 * Author: Guillermo P�rez Trabado <guille@ac.uma.es>
 *
 * Created on 11 de julio de 2018, 17:31
 */

#ifndef PARAMETERS_H
#define	PARAMETERS_H

#include "tableDB.h"

#ifdef	__cplusplus
extern "C"
{
#endif

// Default name of table file name
#define TC_FILENAME "so_dbtable.dat"
    
// Number of entries in a table cache block
#define TC_BLOCK_ENTRIES 4

// Size of a table cache block in bytes
#define TC_CACHE_BLOCK_SIZE ((TC_ENTRY_SIZE)*(TC_BLOCK_ENTRIES))

// Number of blocks in RAM cache
#define TC_CACHE_BLOCKS 4

    
#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETERS_H */

