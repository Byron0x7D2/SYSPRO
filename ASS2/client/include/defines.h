#define MAX_ACTIVE_THREADS 100
#define MAX_NAME_SIZE 100

struct thread_args{
	char host[MAX_NAME_SIZE];
	int port;
	char line[MAX_NAME_SIZE*2];
};

void * thread_function(void * args);