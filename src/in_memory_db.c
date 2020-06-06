#include "in_memory_db.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <util.h>

static table_t db_table = NULL;
static pthread_mutex_t db_table_lock = PTHREAD_MUTEX_INITIALIZER;

void open_table()
{
    int result = pthread_mutex_lock(&db_table_lock);
    CHECK(result == 0);

    if (db_table == NULL)
    {
        db_table = calloc(NUM_RECORDS, sizeof(T_PersistRecord));

        for (int i = 0; i < NUM_RECORDS; i++)
        {
            pthread_rwlock_init(&db_table[i].rw_lock, NULL);
            db_table[i].used = false;
        }
    }
    else
    {
        printf("InMemDB: Table already created\n");
    }

    pthread_mutex_unlock(&db_table_lock);
}

T_PersistRecord *get_next_record(int id, bool write_lock)
{
    CHECK(id >= -1 && id < NUM_RECORDS);

    T_PersistRecord *prec;
    T_PersistRecord *(*access_reg_func)(int);

    if (write_lock)
    {
        access_reg_func = &access_register_write;
    }
    else
    {
        access_reg_func = &access_register_read;
    }

    for (int i = id; i < NUM_RECORDS; i++)
    {
        if (i > -1)
        {
            release_register(i);
        }

        if (i + 1 == NUM_RECORDS)
        { // no next record
            return NULL;
        }

        prec = access_reg_func(i + 1);
        if (prec->used)
        {
            return prec;
        }
    }
}

T_PersistRecord *access_register_read(int id)
{
    printf("Locking read %d\n", id);
    int result = pthread_rwlock_rdlock(&db_table[id].rw_lock);
    CHECK(result == 0);

    return &db_table[id];
}

T_PersistRecord *access_register_write(int id)
{
    printf("Locking write %d\n", id);
    int result = pthread_rwlock_wrlock(&db_table[id].rw_lock);
    CHECK(result == 0);
    return &db_table[id];
}

void release_register(int id)
{
    printf("Releasing %d\n", id);
    pthread_rwlock_unlock(&db_table[id].rw_lock);
}
