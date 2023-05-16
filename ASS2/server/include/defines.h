#pragma once
#include "buffer.h"
#include "log.h"
// struct for arguments to be passed to the thread
typedef struct master_thread_args{
	int portnum, numWorkerThreads;
	Buffer *buffer;
	hash *log;
} master_thread_args;

// struct for arguments to be passed to the thread
typedef struct worker_thread_args{
	Buffer *buffer;
	hash *log;
} worker_thread_args;

void* master_thread_fun(void *arg);
void* worker_thread_fun(void *arg);

extern pthread_mutex_t mtx, logmtx;
extern pthread_cond_t cond_nonempty, cond_nonfull;

#define MAX_NAME_LENGTH 100
#define HASH_SIZE 1091
