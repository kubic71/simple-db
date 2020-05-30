#include "in_memory_db.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <util.h>

static table_t db_table = NULL;  
static pthread_mutex_t db_table_lock = PTHREAD_MUTEX_INITIALIZER;

void open_table() {
    int result = pthread_mutex_lock(&db_table_lock);
    CHECK(result == 0);

    if (db_table == NULL) {
        db_table = calloc(NUM_RECORDS, sizeof(T_PersistRecord));

        for(int i = 0; i < NUM_RECORDS; i++ ) {
            pthread_rwlock_init(&db_table[i].rw_lock, NULL);
            db_table[i].used = false; 
        }
    } else {
        printf("InMemDB: Table already created\n");
    }

    pthread_mutex_unlock(&db_table_lock);
}



T_Record* access_register_read(int id) {
    int result = pthread_rwlock_rdlock(&db_table[id].rw_lock);
    CHECK(result == 0);

    return &db_table[id].record;
}

T_Record* access_register_write(int id) {
    int result = pthread_rwlock_wrlock(&db_table[id].rw_lock);
    CHECK(result == 0);
    return &db_table[id].record;
}

void release_register(int id) {
    pthread_rwlock_unlock(&db_table[id].rw_lock);
}





