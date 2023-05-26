#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#include "../include/defines.h"
#include "../include/buffer.h"
#include "../include/log.h"


/* Function to send message to socket */
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

/* Function to read message from socket */
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

		if(string[bytes_read - 1] == '\n'){
			string[bytes_read - 1] = '\0';
			break;
		}
	}

	return 0;
}

/* Function that the worker threads run 
While active:
	Receive connection from buffer
	Exchenge messages with client
*/
void *worker_thread_fun(void *arg){

	char name[MAX_NAME_LENGTH], vote[MAX_NAME_LENGTH], buf[MAX_NAME_LENGTH*2];
	int ret;
	Buffer *buffer = ((worker_thread_args *) arg)->buffer;
	hash *log = ((worker_thread_args *) arg)->log;

	sigset_t set; // mask so signals go to master thread
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	
	while(!time_to_die){

		pthread_mutex_lock(&mtx);
		while(!buffer_count(buffer)){
			pthread_cond_wait(&cond_nonempty, &mtx); // wait for buffer to have something
			if(time_to_die){ // if time to die, exit
				pthread_mutex_unlock(&mtx); 
				// pthread_exit(NULL);
				return (void*) 0;
			}
		}

		int value = buffer_remove(buffer); // get connection from buffer
		pthread_mutex_unlock(&mtx);
		pthread_cond_signal(&cond_nonfull);


		if(send_message(value, "SEND NAME PLEASE\n") < 0){
			close(value);
			continue;
		}		

		if(read_message(value, name) < 0){
			close(value);
			continue;
		}

		pthread_mutex_lock(&logmtx);
		ret = hash_lookup(log, name); // check if already voted
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

			pthread_mutex_lock(&logmtx);
			hash_insert(log, name, vote); // insert vote in log
			pthread_mutex_unlock(&logmtx);


			sprintf(buf, "VOTE for Party %s RECORDED\n", vote);
			send_message(value, buf);
						
		}


		// close connection
		shutdown(value, SHUT_RDWR);
		close(value);

	}
	// pthread_exit(NULL);
	return (void*) 0;
}