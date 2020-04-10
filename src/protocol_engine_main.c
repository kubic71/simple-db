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


//#define server_IP "192.168.0.1"
#define myPort 8000
//#define backlog = 5

int main(int argc, char** argv) {
    
    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    
    memset(&my_addr, 0, sizeof(struct sockaddr_in)); //clear structure
    
    my_addr.sin_family = AF_INET;           // address family AF_INET
    my_addr.sin_port = myPort;              // set my port e.g. 8000
    my_addr.sin_addr.s_addr = INADDR_ANY;   //set localhost IP (It is address 0.0.0.0 - why?????))
    
    char str [INET_ADDRSTRLEN];
    printf("Server: %s\n", inet_ntop(my_addr.sin_family, &my_addr.sin_addr.s_addr , str, INET_ADDRSTRLEN));
    
    
    /*
     * Creation socket. Socket() function return file descriptor
     */
    int socket_res = socket(PF_INET, SOCK_STREAM, 0);
        if( socket_res == -1){
            perror("socket");
        }
        else
            printf("Socket %d\n", socket_res);
    
 
    /* 
     * If we want to type IP address we can use this part of code. 
     *
    //inet_pton covert string to address structure
    //Setting server_Ip to my_addr structure. 
     * 
    int inet_res;
    unsigned char server_addr[12];
    
    inet_res = inet_pton(my_addr.sin_family, server_IP, server_addr);
       if (inet_res <= 0) {
               if (inet_res == 0){
                   fprintf(stderr, "Not in presentation format");
               }
               else{
                   perror("inet_pton");
               exit(EXIT_FAILURE);
               }
           }
        else{
            //inet_ntop allow to convert address to string. INET_ADDRSTRLEN is format of IPv4
            printf("Server: %s\n", inet_ntop(my_addr.sin_family, server_addr, str, INET_ADDRSTRLEN));
        }
    */
    
    
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
    int listen_backlog = 5;    //Number of waiting calls
    
    int listen_res = listen(socket_res, listen_backlog);
        if(listen_res== -1){
            perror("listen");
            exit(EXIT_FAILURE);
        }
    
        /* In case error "Address is already use" uncomment this part of code.
         * The error may mean that port is still ocupied in kernel. This code solve the problem. 
        int yes=1;
       // zgub denerwuj ˛acy komunikat bł˛edu "Address already in use"
       if (setsockopt(socket_res, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
       perror("setsockopt");
       exit(1);
         }
        */
    
    
    char *SQL_command [100];                //How we get SQL_command (as main argument??) 
    int recvLen = strlen(&SQL_command);
   
    pid_t pid;    
    int status;
    int code;
    int signum;
    int addRecv_res = 0;
    
    while(1)  {      //main loop accept()
    
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

    // <PRE-FORK COMMON CODE HERE>

    /***************************************************************************/

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
    if (pid == 0)
    {
        // <POST-FORK CHILD ONLY CODE HERE>

        close(socket_res);    //In child procces we can close main socket and use only 
                              // new socket        
        
        /* Here we can recive SQL command from buffer (SQL_command) (for streamming sockets!)
         * 0 - it's flag
         * recv_res means amount of byte which were sent
         * 
         * We will recive data untill we get they all.
         * We have to create data structure to know how many bytes the data have??? 
         */
        while(addRecv_res < recvLen){
            //change socket_res on newSock_res!!
            int recv_res = recv(newSocket_res, SQL_command, recvLen, 0);
            if( recv_res == -1){   
                perror("recv");
                continue;
                //exit(EXIT_FAILURE);
            }
            addRecv_res += recv_res;
        }
        
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
        
          char *buf [100];          //Data form database
          int sendLen = sizeof(buf);
          int addSend_res = 0;
          
        /*
         * We will send data until we send they all.
         * 
         */
        while(addSend_res != sendLen){
            int send_res = send(newSocket_res, buf, sendLen, 0);
            if( send_res == -1){   
                perror("send");
                continue;
                //exit(EXIT_FAILURE);
            }
            addSend_res += send_res;
        }
        
          close(newSocket_res);         //After transaction we can close newSocket.
          EXIT(0);
     
    }
    else
    {
        // <POST-FORK PARENT ONLY CODE HERE>
        
        close(newSocket_res);       //Parent proccess dont need to newSocket

        fprintf(stderr, "parent: waiting for children\n", getpid());

        //  Usual parent tasks are controlling the life of children after creation:
        //      Wait for children to end and notifying somebody about status.
        //      Killing children before parent's exit in order to leave system in a clean state.
        //      Stop and continue children in order to be smart with CPU and RAM resources.
        //      Change children priority dinamically to allow smarter scheduling of CPU.

        // This call notifies not only termination but also state changes from children
        /*
	while ((pid = waitpid(-1, &status, WUNTRACED | WCONTINUED)) != -1)
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
        /***************************************************************************/
       */
    }
    }
    
     
    
    return (EXIT_SUCCESS);
}



