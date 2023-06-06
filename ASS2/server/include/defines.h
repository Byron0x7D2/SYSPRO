#pragma once
#include "buffer.h"
#include "log.h"
#include "list.h"

// struct for arguments to be passed to the master thread
typedef struct master_thread_args{
	int portnum, numWorkerThreads;
	Buffer *buffer;
	hash *log;
} master_thread_args;

// struct for arguments to be passed to the worker thread
typedef struct worker_thread_args{
	Buffer *buffer;
	hash *log;
} worker_thread_args;

void* master_thread_fun(void *arg);
void* worker_thread_fun(void *arg);

extern pthread_mutex_t mtx, logmtx; // mutexes
extern pthread_cond_t cond_nonempty, cond_nonfull; // condition variables

extern Pending *pending; // list of pending names

extern int sock, time_to_die; // socket and flag for master thread and worker threads to die

#define MAX_NAME_LENGTH 100 // max length of a name
#define HASH_SIZE 1091 // size of hash table
#define MAX_ACTIVE_CONNECTIONS 128 // max number of active connections
