/*
* New-Style Daemons
* For modern services for Linux
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void daemonize();

int main(int argc, char *argv[]){
	daemonize();
	while(1){
		/*Code for the Storage Engine daemon*/
	}
	return EXIT_SUCCESS;
}

void daemonize(){
	//Fork off the parent process
	pid_t pid = fork();

	/*An error occurred*/
	if(pid < 0)
		exit(EXIT_FAILURE);
	/*Success: let the parent terminate*/
	if (pid > 0)
		exit(EXIT_SUCCESS);
	/*On success: The child process becomes session leader*/
	if (setsid() < 0)
		exit(EXIT_FAILURE);

	/*Catch, ignore and handle signals*/
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	/* Or, use this, recommendable:
	sigaction(int signum, const struct sigaction *act, 
			  struct sigaction *oldact)
	//signum -> specifies the signal
	//act    -> the new action
	//oldact -> old action saved if no NULL
	struct sigaction {
		void     (*sa_handler)(int);
        void     (*sa_sigaction)(int, siginfo_t *, void *);
        sigset_t   sa_mask;
        int        sa_flags;
        void     (*sa_restorer)(void);
	};
	*/

	/*Fork off a second time*/
	pid = fork();

	if (pid < 0)
		exit(EXIT_FAILURE);

	/*Set new file permissions*/
	umask(0);

	/*Change the working directory to the root directory*/
	/*or another appropiate directory*/
	chdir("/");

	/* Close all open file descriptors */
	int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
}