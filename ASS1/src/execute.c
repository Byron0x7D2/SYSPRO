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


void execute(char **argv, char *srcfile, char *destfile, int append){

	if(strcmp(argv[0], "cd") == 0){
		execute_cd(argv);
		return;
	}

	int fdsrc = STDIN_FILENO;
	if(srcfile[0]){
		fdsrc = open(srcfile, O_RDONLY);
		if(fdsrc == -1){
			perror("open");
			exit(EXIT_FAILURE);
		}
	}
	int fddest = STDOUT_FILENO; 
	if(destfile[0]){
		if(append) fddest = open(destfile, O_WRONLY | O_APPEND | O_CREAT, 0666);
		else fddest = open(destfile, O_WRONLY | O_TRUNC | O_CREAT, 0666);
		if(fddest == -1){
			perror("open");
			exit(EXIT_FAILURE);
		}
	}


	pid_t pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){
		if(fdsrc != STDIN_FILENO){
			if(dup2(fdsrc, STDIN_FILENO) == -1){
				perror("dup2");
				exit(EXIT_FAILURE);
			}
		}
		if(fddest != STDOUT_FILENO){
			if(dup2(fddest, STDOUT_FILENO) == -1){
				perror("dup2");
				exit(EXIT_FAILURE);
			}
		}
		execvp(argv[0], argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	else{
		wait(NULL);
	}
}