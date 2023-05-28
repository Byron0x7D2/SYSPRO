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

/* Takes the line and breaks it to a name and a vote */
void fill(char *line, char *name, char* vote){

	int i = 0;

	while(line[i] != ' '){  // get the name
		name[i] = line[i];
		i++;
	}
	name[i] = ' ';
	i++;

	while(line[i] != ' '){  // get the name
		name[i] = line[i];
		i++;
	}


	name[i] = '\n';
	name[i+1] = '\0';

	i++;
	int j = 0;

	while(line[i] != '\n'){ // get the vote
		vote[j] = line[i];
		i++;
		j++;
	}

	vote[j] = '\n';
	vote[j+1] = '\0';
}

/* Send a message to the socket */
int send_message(int sock, char *msg){

	int msglen = strlen(msg);
	int bytes_sent = 0;

	while(bytes_sent < msglen){

		int ret = write(sock, msg + bytes_sent, msglen - bytes_sent);

		if(ret < 0){
			printf("Error writing to socket\n");
			return -1;
		}

		bytes_sent += ret;
	}
	return 0;
}

/* Read a message from the socket */
int read_message(int sock, char *string){

	int bytes_read = 0;

	while(bytes_read < MAX_NAME_SIZE){

		int ret = read(sock, string + bytes_read, MAX_NAME_SIZE - bytes_read);

		if(ret < 0){
			printf("Error reading from socket\n");
			return -1;
		}

		bytes_read += ret;
		if(string[bytes_read - 2] == 13){ // stupid carriage return from telnet
			string[bytes_read - 2] = '\0';
			break;
		}

		if(string[bytes_read - 1] == '\n'){ // normal case
			string[bytes_read - 1] = '\0';
			break;
		}
	}

	return 0;
}


/* Thread function, connects to the server and casts the vote */
void * thread_function(void * args){

	int port = ((struct thread_args *)args)->port;
	char *host = ((struct thread_args *)args)->host;
	char *line = ((struct thread_args *)args)->line;
	int sock = 0;

	char name[MAX_NAME_SIZE];
	char vote[MAX_NAME_SIZE];

	if(strlen(line) == 1)return (void*) 0;
	fill(line, name, vote);

	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;

 
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // create socket
		printf("Error creating socket\n");
		// pthread_exit(NULL);
		return (void*) 0;
	}
 
	if((rem = gethostbyname(host)) == NULL){ // get host
		printf("Error getting host\n");
		close(sock);
		// pthread_exit(NULL);
		return (void*) 0;
	}

	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);

	if(connect(sock, serverptr, sizeof(server)) < 0){ // connect to server
		printf("Error connecting\n");
		close(sock);
		// pthread_exit(NULL);
		return (void*) 0;
	}



	char buffer[MAX_NAME_SIZE];

	read_message(sock, buffer); // read SEND NAME PLEASE

	send_message(sock, name);  // send name

	read_message(sock, buffer); // read SEND VOTE PLEASE or ALREADY VOTED

	if(strcmp(buffer, "ALREADY VOTED") != 0){
		send_message(sock, vote); // send vote
		read_message(sock, buffer); // read VOTE for Party XYZ RECORDED
	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	// pthread_exit(NULL);
	return (void*) 0;
}