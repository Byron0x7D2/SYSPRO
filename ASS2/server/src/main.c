#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/defines.h"
#include "../include/buffer.h"
#include "../include/log.h"
#include "../include/list.h"
#include <signal.h>

// Mutexe and condition variable initialization
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logmtx = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;

// global flag for threads to go die
int time_to_die = 0;

// Pending names to be inserted 
Pending *pending = NULL;

// cntrl c handler
void sigint_handler(int signo){

	time_to_die = 1;

	close(sock);
	pthread_cond_broadcast(&cond_nonempty);
	pthread_cond_broadcast(&cond_nonfull);
	pthread_mutex_unlock(&mtx);

}

/* Fill arguments from command line */
int fill_args(int argc, char ** argv, int *portnum, int *numWorkerthreads, int *bufferSize, char *poll_log, char * poll_stats){
	if(argc < 6) {printf("Not enough arguments\n"); return 0;}
	*portnum = atoi(argv[1]);
	*numWorkerthreads = atoi(argv[2]);
	*bufferSize = atoi(argv[3]);
	if(numWorkerthreads <= 0 || bufferSize <= 0) {printf("Invalid arguments\n"); return 0;}
	strcpy(poll_log, argv[4]);
	strcpy(poll_stats, argv[5]);
	return 1;
}

/* Main function, as per the instructions  
It creates a master thread
It creates a buffer data type
It updates the log file with the statisticks

In addition:
We initialize the signal handler
*/
int main(int argc, char **argv){

	Buffer *buffer;
	hash *logh;

	struct sigaction act;
	act.sa_handler = sigint_handler; // set the handler
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	int portnum, bufferSize, numWorkerthreads;
	char poll_log[100], poll_stats[100];
	pthread_t master_thread;

	if(!fill_args(argc, argv, &portnum, &numWorkerthreads, &bufferSize, poll_log, poll_stats)) return 0;

	// oempty the log file
	FILE *fp = fopen(poll_log, "w");
	fclose(fp);

	// initialize buffer and hash table
	buffer = buffer_init(bufferSize);
	logh = hash_create(poll_log);

	// arguments for master thread initialization
	master_thread_args master_args ;
	master_args.portnum = portnum;
	master_args.numWorkerThreads = numWorkerthreads;
	master_args.buffer = buffer;
	master_args.log = logh;


	// create master thread
	if(pthread_create(&master_thread, NULL, master_thread_fun, (void *) &master_args)){
		printf("Error creating master thread\n");
		return 0;
	}

	// pthread sig mask so SIGINT can unblock accept
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);


	// wait for master thread to finish
	pthread_join(master_thread, NULL);

	// update log file with statistics
	poll_stats_fun(logh, poll_stats);

	buffer_destroy(buffer);
	hash_destroy(logh);

	pthread_mutex_destroy(&mtx);
	pthread_mutex_destroy(&logmtx);
	pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);


}