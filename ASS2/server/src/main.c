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

pthread_t master_thread;
pthread_t* worker_threads;

Buffer *buffer;
hash *logh;
char * output;

int numWorkerthreads;

// cntrl c handler
void sigint_handler(int signo){

	poll_stats(logh, output);

	pthread_cancel(master_thread);

	for(int i = 0; i < numWorkerthreads; i++){
		pthread_cancel(worker_threads[i]);
	}

	close(sock);
	buffer_destroy(buffer);
	hash_destroy(logh);
	free(worker_threads);
	exit(0);
}


int fill_args(int argc, char ** argv, int *portnum, int *numWorkerthreads, int *bufferSize, char *poll_log, char * poll_stats){
	if(argc < 6) {printf("Not enough arguments\n"); return 0;}
	*portnum = atoi(argv[1]);
	*numWorkerthreads = atoi(argv[2]);
	*bufferSize = atoi(argv[3]);
	if(numWorkerthreads <= 0 || bufferSize <= 0) {printf("Invalid arguments\n"); return 0;}
	strcpy(poll_log, argv[4]);
	strcpy(poll_stats, argv[5]);
	output = poll_stats;
	return 1;
}

int main(int argc, char **argv){
	signal(SIGINT, sigint_handler);
	int portnum, bufferSize;
	char poll_log[100], poll_stats[100];

	if(!fill_args(argc, argv, &portnum, &numWorkerthreads, &bufferSize, poll_log, poll_stats)) return 0;

	buffer = buffer_init(bufferSize);
	logh = hash_create(poll_log);

	// fill the struct for the master thread to take as argument
	master_thread_args master_args ;
	master_args.portnum = portnum;
	master_args.numWorkerThreads = numWorkerthreads;
	master_args.buffer = buffer;
	master_args.log = logh;

	worker_threads = malloc(sizeof(pthread_t) * numWorkerthreads);


	// create master thread
	if(pthread_create(&master_thread, NULL, master_thread_fun, (void *) &master_args)){
		printf("Error creating master thread\n");
		return 0;
	}

	//  end master thread
	pthread_join(master_thread, NULL);

	if(buffer) buffer_destroy(buffer);
	if(logh) hash_destroy(logh);

}