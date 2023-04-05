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


void execute(char *exe, char **arguments){
	pid_t pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){
		execvp(exe, arguments);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	else{
		wait(NULL);
	}
}