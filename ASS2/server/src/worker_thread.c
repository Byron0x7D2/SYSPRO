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
#include "../include/log.h"

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

int read_message(int sock, char *string){
	int bytes_read = 0;
	while(bytes_read < MAX_NAME_LENGTH){
		int ret = read(sock, string + bytes_read, MAX_NAME_LENGTH - bytes_read);
		if(ret < 0){
			printf("Error reading from socket\n");
			return -1;
		}
		bytes_read += ret;
		if(string[bytes_read - 2] == 13){ // stupid carriage return from telnet
			string[bytes_read - 2] = '\0';
			break;
		}
	}

	return 0;
}

void *worker_thread_fun(void *arg){
	char name[MAX_NAME_LENGTH], vote[MAX_NAME_LENGTH], buf[MAX_NAME_LENGTH*2];
	int ret;
	Buffer *buffer = ((worker_thread_args *) arg)->buffer;
	hash *log = ((worker_thread_args *) arg)->log;

	
	while(1){

		pthread_mutex_lock(&mtx);
		while(!buffer_count(buffer)){
			pthread_cond_wait(&cond_nonempty, &mtx);
		}
		int value = buffer_remove(buffer);
		pthread_mutex_unlock(&mtx);
		pthread_cond_signal(&cond_nonfull);

		// send message

		if(send_message(value, "SEND NAME PLEASE\n") < 0){
			close(value);
			continue;
		}

		// receive name	
		

		if(read_message(value, name) < 0){
			close(value);
			continue;
		}
		// printf("Received name: %s\n", name);

		pthread_mutex_lock(&logmtx);
		ret = hash_lookup(log, name);
		pthread_mutex_unlock(&logmtx);

		if(ret){
			send_message(value, "ALREADY VOTED\n");
			close(value);
			continue;
		}else{
			
			if(send_message(value, "SEND VOTE PLEASE\n") < 0){
				close(value);
				continue;
			}

			if(read_message(value, vote) < 0){
				close(value);
				continue;
			}
			// printf("Received vote: %s\n", vote);

			pthread_mutex_lock(&logmtx);
			hash_insert(log, name, vote);
			pthread_mutex_unlock(&logmtx);


			sprintf(buf, "VOTE for Party %s RECORDED\n", vote);
			// printf("Sending message: %s\n", buf);
			send_message(value, buf);
						
		}


		// close connection
		close(value);

	}

}