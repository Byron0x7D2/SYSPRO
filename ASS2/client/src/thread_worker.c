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
#include <sys/types.h>
#include <netdb.h>




void fill(char *line, char *name, char* vote){
	int i = 0;
	while(line[i] != ' '){
		name[i] = line[i];
		i++;
	}
	name[i] = '\n';
	name[i+1] = '\0';
	i++;
	int j = 0;
	while(line[i] != '\n'){
		vote[j] = line[i];
		i++;
		j++;
	}
	vote[j] = '\n';
	vote[j+1] = '\0';
}

void send_message(int sock, char *msg){
	int msglen = strlen(msg);
	int sent = 0;
	while(sent < msglen){
		int n = send(sock, msg+sent, msglen-sent, 0);
		if(n < 0){
			printf("Error sending message\n");
			return;
		}
		sent += n;
	}
}

char * receive_message(int sock, char*buffer){
	int n = recv(sock, buffer, MAX_NAME_SIZE, 0);
	if(n < 0){
		printf("Error receiving message\n");
		return NULL;
	}
	buffer[n] = '\0';
	return buffer;
}

void * thread_function(void * args){
	int port = ((struct thread_args *)args)->port;
	char *host = ((struct thread_args *)args)->host;
	char *line = ((struct thread_args *)args)->line;
	int sock = 0;

	char name[MAX_NAME_SIZE];
	char vote[MAX_NAME_SIZE];
	fill(line, name, vote);

	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;


	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket\n");
		pthread_exit(NULL);
	}

	if((rem = gethostbyname(host)) == NULL){
		printf("Error getting host\n");
		pthread_exit(NULL);
	}

	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);

	if(connect(sock, serverptr, sizeof(server)) < 0){
		printf("Error connecting\n");
		pthread_exit(NULL);
	}



	char buffer[MAX_NAME_SIZE];
	// printf("Thread %d startng\n", id);
	receive_message(sock, buffer);
	// printf("Thread %d received %s\n", id, buffer);
	send_message(sock, name);
	// printf("Thread %d sent %s\n", id, name);
	receive_message(sock, buffer);
	// printf("Thread %d received %s\n", id, buffer);
	if(strcmp(buffer, "ALREADY VOTED") != 0){
		send_message(sock, vote);
		// printf("Thread %d sent %s\n", id, vote);
	}

	// printf("Thread %d exiting\n", id);

	close(sock);

	pthread_exit(NULL);
}