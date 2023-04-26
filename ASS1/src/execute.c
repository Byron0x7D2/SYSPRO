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
#include "../include/hash.h"
#include "../include/circulararray.h"
#include "../include/parsing.h"
#include <signal.h>

/* Directory change,
 having finished with everything one would imagine I would just put it in the execute function body but I'm not gonna do it, not sure why */
void execute_cd(char **argv){

	if(chdir(argv[1]) == -1){
			perror("chdir");
			exit(EXIT_FAILURE);
	}
	return;
}

/* Executes the executable stored in argv[0],
or if it is a special input, does that */
pid_t execute(char **argv, char *srcfile, char *destfile, int append, int force_read, int force_write, int other_end, hash *h, circulararray *ca){

	if(strcmp(argv[0], "fg") == 0){ //fg, bring back last stopped process
		pid_t temp;
		if(bg > 0) kill(bg, SIGCONT);
		temp = bg;
		bg = -1;
		return temp;
	}

	if(strcmp(argv[0], "cd") == 0){ // cd, call execute_cd
		execute_cd(argv);
		return -1;  // -1 is a special value that tells the parent not to wait for the child
	}

	if(strcmp(argv[0], "createalias") == 0){ // createalias, saves alias

		if(argv[1] && argv[2]){

			hash_insert(h, argv[1], argv[2]);
		}
		return -1; 
	}

	if(strcmp(argv[0], "destroyalias") == 0){ // destroyalias, deletes alias

		if(argv[1]){

			hash_delete(h, argv[1]);
		}
		return -1;
	}

	if(strcmp(argv[0], "myhistory") == 0){ // myhistory

		if(argv[1]){ // recall from history command and execute it

			int n = atoi(argv[1]);
			char *com = circulararray_get(ca, n); // get command from history

			if(com) {

				char buf[MAX_INPUT_LENGTH];  // write command to file so that it can be read by the main loop
				snprintf(buf, sizeof(buf), "%s/.newinput", getenv("HOME"));
				FILE *fptr = fopen(buf, "w+");
				if(!fptr){
					return -1;
				}

				fprintf(fptr, "%s", com); // write command to file
				fclose(fptr);

				return -2; // -2 is a special value that tells the parent to change input
			}
				
		}

		circulararray_print(ca); // print history

		return -1;
	}


	int fdsrc = STDIN_FILENO; // if there is a redirection, open the file and set the file descriptor
	if(force_read != -1) fdsrc = force_read;
	else if(srcfile[0]){
		fdsrc = open(srcfile, O_RDONLY);
		if(fdsrc == -1){
			perror("open");
			exit(EXIT_FAILURE);
		}
	}
	
	int fddest = STDOUT_FILENO; 
	if(force_write != -1) fddest = force_write;
	else if(destfile[0]){
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

	if(pid == 0){ // child

		if(fdsrc != STDIN_FILENO){  // manage redirection	
			close(STDIN_FILENO);
			if(dup2(fdsrc, STDIN_FILENO) == -1){
				perror("dup2");
				exit(EXIT_FAILURE);
			}
			close(fdsrc);
			
		}

		if(fddest != STDOUT_FILENO){
			close(STDOUT_FILENO);
			if(dup2(fddest, STDOUT_FILENO) == -1){
				perror("dup2");
				exit(EXIT_FAILURE);
			}
			close(fddest);
			
		}

		if(other_end != -1) { // if there is a pipe, close the other end
			close(other_end);
		}

		execvp(argv[0], argv); // execute

		perror("execvp");
		exit(EXIT_FAILURE);

	}else{ // parent
		// close file descriptors	
		if(fdsrc != STDIN_FILENO) close(fdsrc);
		if(fddest != STDOUT_FILENO) close(fddest);
	}

	return pid; // return child pid
}