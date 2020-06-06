#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "transaction_mg.h"
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include "util.h"
#include "query_mq.h"
#include "SQL_parser.h"
#include <signal.h>
#include <pthread.h>
#include "in_memory_db.h"
#include "compare.h"
#include <stdbool.h>

// TODO using threads, maybe not necesary to register signal handlers
static void child_handler(int sig)
{
    pid_t pid;
    int status;

    // WNOHANG --- do not block
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("Transaction handler (pid=%d) exited.\n", pid);
    }
}

static void register_child_handler()
{
    /* Establish handler. */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = child_handler;

    sigaction(SIGCHLD, &sa, NULL);
}

static void connectToStorageEngine()
{
    // for now, we're using simplified version of database which is only in memory and doesn't use any persistent storage
    // when Javier finishes storageEngine, we can integrate it with tm_mg  (transaction manager)
    // nevertheless, the API should be very similar
    open_table();
}

static void record_to_str(T_Record *rec, char *str)
{
    // id;age;height;name

    char buffer[sizeof(T_Record)];

    sprintf(buffer, "%d", rec->id);
    strcpy(str, buffer);
    strcat(str, ";");

    sprintf(buffer, "%d", rec->age);
    strcat(str, buffer);
    strcat(str, ";");

    sprintf(buffer, "%lf", rec->height);
    strcat(str, buffer);
    strcat(str, ";");

    strcat(str, rec->name);
}

static void handle_select_query(Select_Query *query, char *result)
{
    T_PersistRecord *prec;
    int id = -1;

    // there is some overhead when representing numbers as strings
    char rec_str[sizeof(T_Record) + 100];

    while ((prec = get_next_record(id, false)) != NULL)
    {
        if(query->all || satisfy_constraint(&prec->record, &query->constraint)) {
            record_to_str(&prec->record, rec_str);
            strcat(result, rec_str);
            strcat(result, "\n");
        }

        // move onto the next record
        id = prec->record.id;
    }

}

static void handle_delete_query(Delete_Query *query, char *result)
{
    
    T_PersistRecord *prec;
    int id = -1;
    int deleted_num = 0;

    while ((prec = get_next_record(id, true)) != NULL)
    {
        
        if(satisfy_constraint(&prec->record, &query->constraint)) {
            prec->used = false;    
            deleted_num++;
        }

        // move onto the next record
        id = prec->record.id;
    }

    sprintf(result, "Deleted %d records\n", deleted_num);
}

static void handle_insert_query(Insert_Query *query, char *result)
{
    T_PersistRecord* prec = access_register_write(query->record.id);

    prec->used = true;
    prec->record.id = query->record.id;
    prec->record.age = query->record.age;
    prec->record.height = query->record.height;
    strcpy(prec->record.name, query->record.name);

    sprintf(result, "Insert OK: %d;%d;%lf;%s\n", prec->record.id, prec->record.age, prec->record.height, prec->record.name);

    release_register(query->record.id);
    

}

static void handle_update_query(Update_Query *query, char *result)
{
    T_PersistRecord *prec;
    int id = -1;
    int updated_num = 0;

    while ((prec = get_next_record(id, true)) != NULL)
    {
        if(satisfy_constraint(&prec->record, &query->constraint)) {
            switch (query->fieldId)
            {
            case ID:
                prec->record.id = query->val.id; 
                break;
            
            case AGE:
                prec->record.age = query->val.age; 
                break;
            
            case HEIGHT:
                prec->record.height = query->val.height; 
                break;
            
            case NAME:
                strcpy(prec->record.name, query->val.name);
                break;
            }

            updated_num++;
        }

        // move onto the next record
        id = prec->record.id;
    }

    sprintf(result, "Updated %d records\n", updated_num);
}

// entry point of the thread that handles incoming queries
void *handle_query(void *arg)
{
    connectToStorageEngine();

    query_msg_t *query_msg = (query_msg_t *)arg;

    char result_str[RESULT_MSG_SIZE];

    switch (query_msg->query.type)
    {
    case SELECT:
        handle_select_query((Select_Query *)&query_msg->query.query, result_str);
        break;
    case INSERT:
        handle_insert_query((Insert_Query *)&query_msg->query.query, result_str);
        break;
    case DELETE:
        handle_delete_query((Delete_Query *)&query_msg->query.query, result_str);
        break;
    case UPDATE:
        handle_update_query((Update_Query *)&query_msg->query.query, result_str);
        break;
    default:
        perror("Unknown query type");
        break;
    }

    // TODO do the transaction here and collect results

    // sprintf(result_str, "Echo to process pid=%d\nResults of a request of type %s\n", query_msg->pid, QUERYTYPE_TO_STR(query_msg->query.type));

    char q_name[sizeof(RESULTS_QUEUE_NAME) + 10];
    sprintf(q_name, "%s.%d", RESULTS_QUEUE_NAME, query_msg->pid);

    struct mq_attr res_mq_attr = RESULT_QUEUE_ATTR;
    mqd_t result_mq = mq_open(q_name, O_CREAT | O_WRONLY, QUEUE_PERMS, &res_mq_attr);
    CHECK((mqd_t)-1 != result_mq);
    mq_send(result_mq, result_str, RESULT_MSG_SIZE, 0);
    mq_close(result_mq);
    free(arg);

    printf("Thread finished processing query (pid=%d)\n", query_msg->pid);

    // make compiler happy
    return NULL;
}

void transaction_mg_main()
{
    printf("Transaction manager main!\n");

    mqd_t mq;

    struct mq_attr attr = QUERY_QUEUE_ATTR;
    mq = mq_open(QUERY_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMS, &attr);
    CHECK((mqd_t)-1 != mq);

    register_child_handler();

    for (;;)
    {

        query_msg_t *query_msg = malloc(sizeof(query_msg_t));
        /* receive the query message */
        int bytes_read = mq_receive(mq, (char *)query_msg, QUERY_MSG_SIZE, NULL);
        if (bytes_read <= 0)
        {
            printf("Interruped waiting for msg\n");
            continue;
        }

        printf("Received %d bytes from process with pid=%d\n", bytes_read, query_msg->pid);
        printf("Query type: %s\n\n", QUERYTYPE_TO_STR(query_msg->query.type));

        // spawn a thread for each incoming query
        pthread_t thread;
        pthread_create(&thread, 0, handle_query, query_msg);
    }
}