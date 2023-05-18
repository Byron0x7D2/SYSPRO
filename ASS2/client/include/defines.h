#define MAX_ACTIVE_THREADS 100  // Max number of active threads created by client at a time
#define MAX_NAME_SIZE 100 // Max size of a name

struct thread_args{ // Struct to pass arguments to thread function
	char host[MAX_NAME_SIZE];
	int port;
	char line[MAX_NAME_SIZE*2];
};

void * thread_function(void * args);
