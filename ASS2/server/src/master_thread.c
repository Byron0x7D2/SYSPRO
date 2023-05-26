#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/defines.h"
#include "../include/buffer.h"

int sock; // global socket

/* 
	Function that the master thread runs
	Creates socket
	Binds socket
	Listens for connections
	
	Creates worker threads
	While active:
		Accepts connections
		Inserts connections to buffer
		Signals worker threads to work (no free lunch :) )
 */
void* master_thread_fun(void *arg){

	int portnum = ((master_thread_args *) arg)->portnum;
	int numWorkerThreads = ((master_thread_args *) arg)->numWorkerThreads;
	Buffer *buffer = ((master_thread_args *) arg)->buffer;
	hash *log = ((master_thread_args *) arg)->log;

	// create socket
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket\n");
		// pthread_exit(NULL);
		return (void*) 0;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(portnum);
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 

	if(bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0){ // bind
		printf("Error binding socket\n");
		// pthread_exit(NULL);
		return (void*) 0;
	}

	// listen
	if(listen(sock, MAX_ACTIVE_CONNECTIONS) < 0){
		printf("Error listening\n");
		// pthread_exit(NULL);
		return (void*) 0;
	}


	// create worker threads
	worker_thread_args worker_args;
	worker_args.buffer = buffer;
	worker_args.log = log;
	pthread_t worker_threads[numWorkerThreads];

	for(int i = 0; i < numWorkerThreads; i++){ 

		if(pthread_create(&worker_threads[i], NULL, worker_thread_fun, (void *) &worker_args)){
			printf("Error creating worker thread\n");
			// pthread_exit(NULL);
			return (void*) 0;
		}
	}

	int newsock;

	// main loop
	while(!time_to_die){

		if((newsock = accept(sock, NULL, NULL)) < 0){ // accept
			if(time_to_die) break;
			printf("Error accepting connection\n");
			// pthread_exit(NULL);
			return (void*) 0;
		}

		// add new connection to buffer 
		pthread_mutex_lock(&mtx);
		while(buffer_count(buffer) >=  buffer_size(buffer)){
			pthread_cond_wait(&cond_nonfull, &mtx);
			if(time_to_die){
				pthread_mutex_unlock(&mtx);
				break;
			}
		}
		buffer_insert(buffer, newsock);
		pthread_mutex_unlock(&mtx);
		pthread_cond_signal(&cond_nonempty);

	}
	
	close(sock);

	// wait for worker threads to finish
	for(int i = 0; i < numWorkerThreads; i++){
		pthread_join(worker_threads[i], NULL);
	}

	// exit
	// pthread_exit(NULL);
	return (void*) 0;
}