#ifndef QUERY_MQ_H
#define	QUERY_MQ_H

#include "SQL_parser.h"
#define QUERY_MSG_SIZE sizeof(query_msg_t)
#define QUERY_QUEUE_NAME "/query_queue"

typedef struct {
  int pid;   // Serves as an identifier of the process, to which the response should be sent
  SQL_Query query;
  
} query_msg_t;


#endif
