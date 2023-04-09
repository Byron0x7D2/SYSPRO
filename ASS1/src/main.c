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

pid_t pid = -1;

void catch_sigint(){
	if(pid > 0) kill(pid, SIGINT);
	else{
		signal(SIGINT, SIG_DFL);
		kill(getpid(), SIGINT);
	}
}

void catch_sigtstp(){
	if(pid > 0){ 
		pid = -1; 
		kill(pid, SIGTSTP); 
	}else{
		signal(SIGTSTP, SIG_DFL);
		kill(getpid(), SIGTSTP);
	}
}

int main(int argc, char *argv[]){
	int last_status = NL;
	int status;
	pid_t wpid;
	signal(SIGINT, catch_sigint);
	signal(SIGTSTP, catch_sigtstp);
	hash *h = hash_create_and_init();
	while(1){		
		if(last_status == NL)printf("in-mysh-now:>");
		last_status = command(-1,-1, -1, &pid, h);
		if(last_status != AMP && pid > 0){
			do{
				wpid = waitpid(-1, &status, 0);
			}while(wpid != pid);
		}
		if(last_status == MYEXIT) break;
	}

	hash_destroy_and_save(h);
	return 0;
}