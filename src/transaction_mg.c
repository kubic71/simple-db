#include "transaction_mg.h"
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include "util.h"
#include "query_mq.h"
#include "SQL_parser.h"

void transaction_mg_main()
{
    printf("Transaction manager main!\n");

    mqd_t mq;
    struct mq_attr attr;

    query_msg_t query_msg;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = QUERY_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUERY_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    for (;;)
    {
        int bytes_read;

        /* receive the query message */
        bytes_read = mq_receive(mq, (char *)&query_msg, QUERY_MSG_SIZE, NULL);
        CHECK(bytes_read >= 0);
        printf("Received %d bytes from process with pid=%d\n", bytes_read, query_msg.pid);
        printf("Query type: %s\n\n", QUERYTYPE_TO_STR(query_msg.query.type));
    }


    /* cleanup */
    // CHECK((mqd_t)-1 != mq_close(mq));
    // CHECK((mqd_t)-1 != mq_unlink(QUERY_QUEUE_NAME));

}