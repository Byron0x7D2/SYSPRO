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
#include <signal.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logmtx = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;

// cntrl c handler
void sigint_handler(int signo){
	printf("Exiting...\n");
	exit(0);
}


int fillargs(int argc, char ** argv, int *portnum, int *numWorkerthreads, int *bufferSize, char *poll_log, char * poll_stats){
	if(argc < 6) {printf("Not enough arguments\n"); return 0;}
	*portnum = atoi(argv[1]);
	*numWorkerthreads = atoi(argv[2]);
	*bufferSize = atoi(argv[3]);
	if(numWorkerthreads <= 0 || bufferSize <= 0) {printf("Invalid arguments\n"); return 0;}
	strcpy(poll_log, argv[4]);
	strcpy(poll_stats, argv[5]);
	return 1;
}

int main(int argc, char **argv){
	signal(SIGINT, sigint_handler);
	int portnum, numWorkerthreads, bufferSize;
	char poll_log[100], poll_stats[100];

	if(!fillargs(argc, argv, &portnum, &numWorkerthreads, &bufferSize, poll_log, poll_stats)) return 0;

	// create the buffer
	Buffer *buffer = buffer_init(bufferSize);
	hash *log = hash_create(poll_log);

	// fill the struct for the master thread to take as argument
	master_thread_args *master_args = malloc(sizeof(master_thread_args));
	master_args->portnum = portnum;
	master_args->numWorkerThreads = numWorkerthreads;
	master_args->buffer = buffer;
	master_args->log = log;


	// create master thread
	pthread_t master_thread;
	if(pthread_create(&master_thread, NULL, master_thread_fun, (void *) master_args)){
		printf("Error creating master thread\n");
		return 0;
	}

	//  end master thread
	pthread_join(master_thread, NULL);

	if(buffer) buffer_destroy(buffer);
	if(log) hash_destroy(log);

}