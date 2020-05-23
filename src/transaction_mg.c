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

// entry point of the thread that handles incoming queries
void *handle_query(void *arg)
{
    query_msg_t *query_msg = (query_msg_t *)arg;

    // TODO do the transaction here and collect results

    char result_str[RESULT_MSG_SIZE];
    sprintf(result_str, "Echo to process pid=%d\nResults of a request of type %s\n", query_msg->pid, QUERYTYPE_TO_STR(query_msg->query.type));

    char q_name[sizeof(RESULTS_QUEUE_NAME) + 10];
    sprintf(q_name, "%s.%d", RESULTS_QUEUE_NAME, query_msg->pid);

    struct mq_attr res_mq_attr = RESULT_QUEUE_ATTR;
    mqd_t result_mq = mq_open(q_name, O_CREAT | O_WRONLY, QUEUE_PERMS, &res_mq_attr);
    CHECK((mqd_t)-1 != result_mq);
    mq_send(result_mq, result_str, RESULT_MSG_SIZE, 0);
    mq_close(result_mq);
    free(arg);

    printf("Thread finished processing query (pid=%d)\n", query_msg->pid);
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