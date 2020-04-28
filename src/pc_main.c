/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: daniel
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


void waitForChild(int s)
{
    int status;
    int code;
    int signum;
    pid_t pid;

        // This call notifies not only termination but also state changes from children

	if ((pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) != -1)
        {
            // Status is a multifield value with this information:
            //      Event: Type of state change (EXITED|TERMINATED BY SIGNAL|STOPPED|CONTINUED)
            //      Event information: Number of the signal which terminated/stopped the child
            //      Exit code for exited processes.

            // Use macros to check fields. NEVER USE status directly!!!
            if (WIFEXITED(status))
            {
                code = WEXITSTATUS(status);
                fprintf(stderr, "parent: child %d terminated with exit(%d)\n", pid, code);
            }
            if (WIFSIGNALED(status))
            {
                signum = WTERMSIG(status);
                fprintf(stderr, "parent: child %d kill by signal %d\n", pid, signum);
            }
            if (WIFSTOPPED(status))
            {
                signum = WSTOPSIG(status);
                fprintf(stderr, "parent: child %d stopped by signal %d\n", pid, signum);
            }
            if (WIFCONTINUED(status))
            {
                fprintf(stderr, "parent: child %d continued\n", pid);
            }
        }
}

/*
struct in_addr {
    unsigned long address;      //4 bytes IP address
}in_addr1;

//its necessary??
struct sockaddr {
    unsigned short address_family;      //its necessary??
    char address_data;                  // protocol address
}sockaddr1;

struct sockaddr_in {
    short int sin_family;               //address family
    unsigned short int sin_port;        //port number
    struct in_addr sin_addr;            //internet address
    unsigned char siz_zero[8];
}my_addr;

 */


#define MYPORT 8000
#define BACKLOG 10
#define MAXQUERY 100

int main(int argc, char** argv) {
    
    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    
    memset(&my_addr, 0, sizeof(struct sockaddr_in)); //clear structure
    
    /*
     * Creation socket. Socket() function return file descriptor
     */
    int socket_res = socket(PF_INET, SOCK_STREAM, 0);
        if( socket_res == -1){
            perror("socket");
        }
        else
            printf("Socket %d\n", socket_res);

	my_addr.sin_family = AF_INET;           	// address family AF_INET (IPv4)
	my_addr.sin_port = htons(MYPORT);           // set my port e.g. 8000 -> it does not work
	my_addr.sin_addr.s_addr = INADDR_ANY;   	//set localhost IP

	char str [INET_ADDRSTRLEN];
	printf("Server: %s\n", inet_ntop(my_addr.sin_family, &my_addr.sin_addr.s_addr , str, INET_ADDRSTRLEN));


	/* In case error "Address is already use" uncomment this part of code.
	 * The error may mean that port is still ocupied in kernel. This code solve the problem.
	 */
	int yes=1;
	if (setsockopt(socket_res, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	perror("setsockopt");
	exit(1);
	}


    /*
     * bind() assigns the address specified by my_addr to the socket referred to by the file descriptor(socket_res).
	*/
    int bind_res = bind(socket_res, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
        if(bind_res == -1){
            perror("bind");
            exit(EXIT_FAILURE);
        }

    /*
     * Listening the port which was choose automatically by used bind() command
     */
    int listen_res = listen(socket_res, BACKLOG);
        if(listen_res== -1){
            perror("listen");
            exit(EXIT_FAILURE);
        }
    
    char sqlCommand[MAXQUERY];
    int addRecv_res = 0;
    pid_t pid;
    
    signal(SIGCHLD, waitForChild);

    while(1)  {      						//main loop accept()
    
		/*
		 * Function accept() accept a call from queue
		 * newSock_res is new socket descriptor.
		 */
		int sockaddr_size = sizeof(struct sockaddr_in);

		int newSocket_res = accept(socket_res, (struct sockaddr *) &client_addr, &sockaddr_size);
			if(newSocket_res == -1){
				perror("accept");
				continue;
			}

		/***************************************************************************/
		/* This is the parent process before forking. Every change done is this section
		 * will affect both the parent and the child (inherited).
		 */

		pid = fork();
		/* Errors in fork mean that there is no child process. Parent is alone. */
		if (pid == -1)
		{
			/* Error in fork: This is severe!!! Abort further processing!!! */
			fprintf(stderr, "parent: error in fork\n");
			exit(EXIT_FAILURE);
		}

		/* Fork has succeeded. Sentences after fork are being executed both by parent
		 * and child but in their respective process contexts (isolated from each other).
		 */
		printf("FORK \n");
		if (pid == 0)
		{
			// <POST-FORK CHILD ONLY CODE HERE>

			close(socket_res);    		//In child process use only newSocket_res, so socket_res can be close
			printf("Child \n");

			/* Here we can receive SQL command from buffer (SQL_command) (for streaming sockets!)
			 * 0 - it's flag
			 * recv_res means amount of byte which were sent
			 *
			 * We will receive data until we get they all.
			 * We have to create data structure to know how many bytes the data have???
			 */

			//while(addRecv_res < MAXQUERY){				//It should be length of query ??

				int recv_res = recv(newSocket_res, sqlCommand, MAXQUERY, 0);
				if( recv_res == -1){
					perror("recv");
					continue;
				}
			//	addRecv_res += recv_res;
			//}

			printf("SQL COMMAND: %s \n", sqlCommand);

			/*
			 *
			 *
			 * Waiting for data from database...
			 *
			 *
			 */

			 /* Here we can send the data to client(for streamming sockets!!! To datagram socket use recvfrom!)
			 * 99 - it's buffer length,
			 * 0 - it's flag
			 * if result >0 means amount of byte which were sent
			 */

			  char *buf [100];          	//Data form database
			  int sendLen = sizeof(buf);
			  //int addSend_res = 0;

			/*
			 * We will send data until we send they all.
			 *
			 */
			//while(addSend_res != sendLen){
				int send_res = send(newSocket_res, buf, sendLen, 0);
				if( send_res == -1){
					perror("send");
					continue;
					//exit(EXIT_FAILURE);
				}
			//	addSend_res += send_res;
			//}

			close(newSocket_res);         //After transaction we can close newSocket.
			exit(EXIT_FAILURE);
		}
		else
		{
			// <POST-FORK PARENT ONLY CODE HERE>
			//fprintf(stderr, "parent: waiting for children\n", getpid());

			close(newSocket_res);       	//Parent process don't need to newSocket_res
		}
    }
    return (EXIT_SUCCESS);
}



