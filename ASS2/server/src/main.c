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



int time_to_die = 0;


// cntrl c handler
void sigint_handler(int signo){

	time_to_die = 1;

	close(sock);
	pthread_cond_broadcast(&cond_nonempty);
	pthread_cond_broadcast(&cond_nonfull);
	pthread_mutex_unlock(&mtx);

}


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

int main(int argc, char **argv){
	Buffer *buffer;
	hash *logh;
	struct sigaction act;
	act.sa_handler = sigint_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);


	int portnum, bufferSize, numWorkerthreads;
	char poll_log[100], poll_stats[100];
	pthread_t master_thread;

	if(!fill_args(argc, argv, &portnum, &numWorkerthreads, &bufferSize, poll_log, poll_stats)) return 0;

	// open log file
	FILE *fp = fopen(poll_log, "w");
	fclose(fp);

	buffer = buffer_init(bufferSize);
	logh = hash_create(poll_log);

	// fill the struct for the master thread to take as argument
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

	// pthread sig mask
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);


	//  end master thread
	pthread_join(master_thread, NULL);
	poll_stats_fun(logh, poll_stats);

	buffer_destroy(buffer);
	hash_destroy(logh);


}