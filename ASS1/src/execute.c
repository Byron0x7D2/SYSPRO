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

void execute_cd(char **argv){
	if(chdir(argv[1]) == -1){
			perror("chdir");
			exit(EXIT_FAILURE);
	}
	return;
}


void execute(char **argv){
	if(strcmp(argv[0], "cd") == 0){
		execute_cd(argv);
		return;
	}
	pid_t pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){
		execvp(argv[0], argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	else{
		wait(NULL);
	}
}