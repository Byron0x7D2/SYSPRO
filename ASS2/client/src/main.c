#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/defines.h"
#include <signal.h>

int fill_args(int argc, char ** argv, char *host, int *portnum, char *inputfile){
	if(argc < 4) {printf("Not enough arguments\n"); return 0;}
	strcpy(host, argv[1]);
	*portnum = atoi(argv[2]);
	strcpy(inputfile, argv[3]);
	return 1;
}

int get_lines(char *filename){
	FILE *fp = fopen(filename, "r");
	if(fp == NULL) {printf("Error opening file\n"); return -1;}
	int lines = 0;
	char c;
	while((c = fgetc(fp)) != EOF){
		if(c == '\n') lines++;
	}
	fclose(fp);
	return lines;
}


int main(int argc, char **argv){
	char inputfile[100], host[100];
	int port;

	if(!fill_args(argc, argv, host, &port, inputfile)) return 0;

	int lines = get_lines(inputfile);


	// for each line create a thread up to MAX_THREADS at a time
	pthread_t* threads = malloc(sizeof(pthread_t)*lines);
	struct thread_args* args = malloc(sizeof(struct thread_args)*lines);
	FILE *fp = fopen(inputfile, "r");

	for(int i = 0; i < lines; i++){

		strcpy(args[i].host, host);
		args[i].port = port;		
		if(fp == NULL) {printf("Error opening file\n"); return -1;}
		char line[MAX_NAME_SIZE*2];
		fgets(line, MAX_NAME_SIZE*2, fp);
		strcpy(args[i].line, line);
		args[i].id=i;
	}

	fclose(fp);

	int j, k;
	for(int i = 0; i < lines; ){
		for(j = 0; i < lines && j < MAX_ACTIVE_THREADS; j++, i++){
			pthread_create(&threads[i], NULL, thread_function, (void *)&args[i]);
		}

		for(k = 0; k < j; k++){
			pthread_join(threads[i-j + k], NULL);
		}
	}

	free(threads);
	free(args);

	

}