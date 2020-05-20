#ifndef QUERY_MQ_H
#define	QUERY_MQ_H

#include "SQL_parser.h"
#include <mqueue.h>

#define QUEUE_MAXMSG 10    // Cannot be greater than 10 
#define QUEUE_PERMS ((int)(0666)) 

#define QUERY_MSG_SIZE sizeof(query_msg_t)
#define QUERY_QUEUE_NAME "/query_queue"
#define QUERY_QUEUE_ATTR ((struct mq_attr){0, QUEUE_MAXMSG, QUERY_MSG_SIZE, 0, {0}})


// TODO variable-length message
#define RESULT_MSG_SIZE 1024
#define RESULTS_QUEUE_NAME "/results_queue"
#define RESULT_QUEUE_ATTR ((struct mq_attr){0, QUEUE_MAXMSG, RESULT_MSG_SIZE, 0, {0}})

typedef struct {
  int pid;   // Serves as an identifier of the process, to which the response should be sent
  SQL_Query query;
  
} query_msg_t;


#endif
