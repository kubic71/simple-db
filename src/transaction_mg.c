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

static int responseParts(char data[]){
	int size = strlen(data);
	int parts = size/1024;
	return parts + 1;
}

static struct response_msg_t response_analyzer(char data[], int part) {
    struct response_msg_t response_msg;
	strncpy(response_msg.response, &data[(part - 1) * 1024], 1024);
	return response_msg;
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

            // TODO do the transaction here and collect results
            char result_str[RESULT_MSG_SIZE * 3];
//            sprintf(result_str, "Echo to process pid=%d\nResults of a request of type %s\n", query_msg.pid, QUERYTYPE_TO_STR(query_msg.query.type));
//            for(int i = 0; i < 9; i++){
//            	strncat(result_str, (char) i,1);
//            }
//            sprintf(result_str, "");
            sprintf(result_str, "K30TZLJdCfGWWCNHjSUzfiaiKnAzG7FpiEtYBtlQOO11OrLPxEw7ay4lr9WLDikhFolvPvA3oaSfW6gQvEwacNIQabDJw96iAG3QTwQiiRn2eDZ09pKq0b0cOJdJkwtsYEiSHmJcWLxChOqSmvktiwUNmVCBwlf6attlBl3HAb8Ei2A53bYKCnw5557VnJckOO5inNBl8kdzRHGyL0ucXmtbHWarKl66YNW90dNufbvs7bY7ChrhA9t7cOAkL7q85hGvstMIjNiFuqy6roXt6yzH8nQLPGP7fZi9ioGUG3rrR3y6vP2T0lRsdsGdOIf3FDhKjiRD1waRsVFk78FcXfZ9IIcRn8xzw8KSMLMdFjfyCX0Hi2AP0cVIFzsYcAbu3wde06mqTvHOxM3P7utHKwmftISPkUUyR6AKvXufvgQspJnF12jWnnULPxn8LNBSJ5cTor5DbdgZhBB1huV26rbgIIDQI8i65OajT9OlrJeYLjABsmyNIETpvjQJvvvrFKpQk7BRlayD5Z7uMIdJNDn0U4vQbL0U33GpsbAym0MCoYGd743Ln1qTrZr1i69gxX6Rc87CbN2sSEscLYNSLbJaCOxMDsCjXWGrq2YTGgBFx8TynpNhKF6j4vkOrCGnWBkgRfoG3Qypi1QqsX6aGynWD8YsKATfYzPXjmwOksNNDPaDcslh5NxoeD8I2BaOVFKJOKnqb2OoClJC6ELxECEyGz1SltG1Rhnhuds2Z3yZMWTUMDDg4zKvgq6o1bApvkLtrsT5fehVtZnp1VjbO7ZC4gZXZwtS9RkxY02aSRojueAEwUBPwpk9dlY6xikrLqzrxeZ7tapDmhWArKO4XtKZ4GYe41gdixevdhCLYyfZX1EN1Sn0gznbRTtEsA34N412sCvl0fLRuZwjpDZWx5rn3W5AMuveIlJPmForuDqpOHjs7T7AU9wC8t190yRVSz8Q1DlTaKbnfqpyAsiLrm2YlBauXt9ZmEvA7DmymEFcW6o7DaCJlai5qEVlPeDl");

            char q_name[sizeof(RESULTS_QUEUE_NAME) + 10];
            sprintf(q_name, "%s.%d", RESULTS_QUEUE_NAME, query_msg.pid);

            struct mq_attr res_mq_attr = RESULT_QUEUE_ATTR;
            mqd_t result_mq = mq_open(q_name, O_CREAT | O_WRONLY, QUEUE_PERMS, &res_mq_attr);
            CHECK((mqd_t)-1 != result_mq);

            struct response_msg_t response_msg;
            struct init_response_t init_response;

            //Calc parts of response
            int parts = responseParts(result_str);

            init_response.parts = parts;
            sprintf(init_response.bytes,"%ld",strlen(result_str));
            init_response.pid = query_msg.pid;

            // Sending initial response
			mq_send(result_mq, (char *) &init_response, RESULT_MSG_SIZE, 0);
            printf("Sent %ld bytes in %d parts to pid:%d\n", strlen(result_str), parts, init_response.pid );

            // Sending data
            for(int part = 1; part <= parts; part++) {
            	response_msg = response_analyzer(result_str, part);
//            	printf("Response_struct array:%s", response_msg.response);
    			mq_send(result_mq, (const char *) &response_msg, sizeof(response_msg), 0);
            }
//            mq_send(result_mq, result_str, RESULT_MSG_SIZE, 0);
//            mq_send(result_mq, result_str, sizeof(result_str), 0);
            mq_close(result_mq);
            exit(EXIT_SUCCESS);
        }
    }
}
