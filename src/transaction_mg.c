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

static int responseParts(char dataFromStorage[]){

	int size = sizeof(dataFromStorage);
	int parts = size/1024;
	return parts;
}

static response_msg_t response_analyzer(char dataFromStorage[], query_msg_t query_msg, int parts, int part, int parameter)
{
    response_msg_t response_msg;	//respond_msg
//	int part = 0;
//	int parameter = 0;

	if(parts == 0)
	{
//		response_msg.flag = 0;
//		response_msg.offset = 0;
		response_msg.resID = query_msg;
//		response_msg.totMsgSize = size;
		strcpy(response_msg.response, dataFromStorage);

		return response_msg;
	}
	else if(parts < 0)
	{	//Error
	}
	else {
//		for(part = 0; part <= parts; part++, parameter++){
		/*
			if(part == 0) {
				response_msg.flag = 0;
			}
			else if(part == parts){
				response_msg.flag = 2;
			}
			else {
				response_msg.flag = 1;
			}
		*/
//			response_msg.offset = (parameter * 1024)/8;
			response_msg.resID = query_msg;
//			response_msg.totMsgSize = size;
			strncpy(response_msg.response, dataFromStorage, 1024);

			return response_msg;
//	}
	}
}

void transaction_mg_main()
{
    printf("Transaction manager main!\n");

    mqd_t mq;
    query_msg_t query_msg;

    struct mq_attr attr = QUERY_QUEUE_ATTR;
    mq = mq_open(QUERY_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMS, &attr);
    CHECK((mqd_t)-1 != mq);

    register_child_handler();

    for (;;)
    {

        /* receive the query message */
        int bytes_read = mq_receive(mq, (char *)&query_msg, QUERY_MSG_SIZE, NULL);
        if (bytes_read <= 0)
        {
            printf("Interruped waiting for msg\n");
            continue;
        }

        printf("Received %d bytes from process with pid=%d\n", bytes_read, query_msg.pid);
        printf("Query type: %s\n\n", QUERYTYPE_TO_STR(query_msg.query.type));

        // spawn child process for handling the transaction
        int pid = fork();
        CHECK(pid != -1);
        if (pid == 0)
        {
            mq_close(mq);

            response_msg_t response_msg;
            int parameter = 0;
            int part = 0;
            int parts = 0;

            // TODO do the transaction here and collect results

//            char result_str[RESULT_MSG_SIZE];
//            sprintf(result_str, "Echo to process pid=%d\nResults of a request of type %s\n", query_msg.pid, QUERYTYPE_TO_STR(query_msg.query.type));

            char result_str[] = "Echo to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \n";
//            sprintf(result_str, "Echo to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \nEcho to process pid=\nResults of a request of type \n");

            char q_name[sizeof(RESULTS_QUEUE_NAME) + 10];
            sprintf(q_name, "%s.%d", RESULTS_QUEUE_NAME, query_msg.pid);

            struct mq_attr res_mq_attr = RESULT_QUEUE_ATTR;

            mqd_t result_mq = mq_open(q_name, O_CREAT | O_WRONLY, QUEUE_PERMS, &res_mq_attr);
            CHECK((mqd_t)-1 != result_mq);

            printf("Sizeof(Data):%ld\n", sizeof(result_str));

            parts = responseParts(result_str);

            printf("Data size:%ld 	And amount of parts:%d\n", sizeof(result_str), parts);


            for(part = 0; part <= parts; part++, parameter++) {
            	response_msg = response_analyzer(result_str, query_msg, parts, part, parameter);
				mq_send(result_mq, (const char *) &response_msg.response, sizeof(response_msg.response), 0);
            }

//            mq_send(result_mq, result_str, RESULT_MSG_SIZE, 0);
//            mq_send(result_mq, result_str, sizeof(result_str), 0);
            mq_close(result_mq);
            exit(EXIT_SUCCESS);
        }
    }
}
