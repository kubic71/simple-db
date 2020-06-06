#ifndef IN_MEMORY_DB_H
#define IN_MEMORY_DB_H

#include "table.h"
#include <pthread.h>
#include <stdbool.h>


/* Very basic implementation of in-memory database storage, which will be replaced by Javier's code 
 * There is identity mapping between record's id and its position in memory, because I didn't want to bother with DB index
 * 
 */

#define NUM_RECORDS 10    // for testing purposes 

typedef struct {
    bool used;
    pthread_rwlock_t rw_lock;
    T_Record record;
} T_PersistRecord;


typedef T_PersistRecord* table_t;           
void open_table();



T_PersistRecord* access_register_read(int id);
T_PersistRecord* access_register_write(int id);
void release_register(int id);

/*
 * Move to the next used record 
 * When id=-1 is given, return first record
 * return NULL when there isn't any next record
 */
T_PersistRecord* get_next_record(int id, bool write_lock);

#endif // IN_MEMORY_DB_H