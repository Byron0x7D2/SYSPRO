#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../include/defines.h"
#include "../include/parsing.h"
#include "../include/execute.h"
#include <signal.h>
#include "../include/hash.h"
#include "../include/circulararray.h"

pid_t pid = -1; // global variable to denote the current process running, -1 means that there is no process running 

/* Catch cntl C signal and send it to child process, 
if there is not one, the father kills himself */
void catch_sigint(){
	if(pid > 0) kill(pid, SIGINT);
	else{
		signal(SIGINT, SIG_DFL);
		kill(getpid(), SIGINT);
	}
}

/* Catch cntrl Z signal, same as before */
void catch_sigtstp(){
	if(pid > 0){ 
		pid = -1; // stop waiting for this process to finish
		kill(pid, SIGTSTP); 
	}else{
		signal(SIGTSTP, SIG_DFL);
		kill(getpid(), SIGTSTP);
	}
}

int main(int argc, char *argv[]){

	FILE *input = stdin;
	int last_status = NL;
	int status;
	pid_t wpid;

	signal(SIGINT, catch_sigint); //signal handlers
	signal(SIGTSTP, catch_sigtstp);

	hash *h = hash_create_and_init(); //ADT initializations for aliases and history
	circulararray *ca = circulararray_create_and_init();
 
	while(1){	// untill the user types rip

		if(last_status == NL){
			printf("in-mysh-now:>");
			if(input != stdin) fclose(input);
			input = stdin;
		}

		last_status = command(-1,-1, -1, &pid, h, ca, input);

		if(last_status != AMP && pid > 0){
			do{
				wpid = waitpid(-1, &status, 0); // wait for the child process to finish
			}while(wpid != pid);
		}

		if(last_status == MYINPUT){ // if the user typed myhistory, we need to open the file with the new input and replace the stdin
			char buf[MAX_INPUT_LENGTH];
			snprintf(buf, sizeof(buf), "%s/.newinput", getenv("HOME"));
			input = fopen(buf, "r");
			if(!input){
				perror("fopen");
				exit(EXIT_FAILURE);
			}
		}

		if(last_status == MYEXIT) break;
	}

	circulararray_destroy_and_save(ca); // destroy the ADTs
	hash_destroy_and_save(h);

	return 0;
}