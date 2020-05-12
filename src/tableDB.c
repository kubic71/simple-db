//Javier Marquez Ruiz

/*Why this??
#ifdef	__cplusplus
extern "C"
{
#endif
*/

#include "tableDB.h"

int openTable(){
}

int TC_flushAllBlocks(){
	//Writes all blocks in file and update buffers
}

int TC_writeEntryAsync(int fileIndex, TC_tableEntry_t * entry){
	//Positionate using offset, sizes and the id of the entry.
}

int TC_writeEntrySync(int fileIndex, TC_tableEntry_t * entry){
}

int TC_readEntry(int fileIndex, TC_tableEntry_t *entry){
}

int TC_closeTable(){
}

/* Why this??
#ifdef	__cplusplus
extern "C"
{
#endif
*/