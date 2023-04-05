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


void get_memory(char **input, char **exe, char ***arguments){
	*input = malloc(sizeof(char) * MAX_INPUT_LENGTH);
	*exe = malloc(sizeof(char) * MAX_FILE_LENGTH);
	*arguments = malloc(sizeof(char*) * MAX_ARGUMENTS);
	if(!input || !exe || !arguments){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	// for(int i = 0; i < MAX_ARGUMENTS; i++){
	// 	(*arguments)[i] = malloc(sizeof(char) * MAX_FILE_LENGTH);
	// 	if(!(*arguments)[i]){
	// 		perror("malloc");
	// 		exit(EXIT_FAILURE);
	// 	}
	// }
}

void free_memory(char *input, char *exe, char **arguments){
	free(input);
	free(exe);
	for(int i = 0; i < MAX_ARGUMENTS; i++){
		if(arguments[i]) free(arguments[i]);
		else break;
	}
	free(arguments);
}

void parse_string(char* input, char* exe, char **arguments){
	char *tokens = " \t\n";
	char *token = strtok(input, tokens);
	strcpy(exe, token);

	arguments[0] = malloc(sizeof(char) * MAX_FILE_LENGTH);
	if(!arguments[0]){perror("malloc"); exit(EXIT_FAILURE);} 

	strcpy(arguments[0], token);
	token = strtok(NULL, tokens);
	int i = 1;
	while(token && i < MAX_ARGUMENTS){

		arguments[i] = malloc(sizeof(char) * MAX_FILE_LENGTH);
		if(!arguments[i]){perror("malloc"); exit(EXIT_FAILURE);} 

		strcpy(arguments[i], token);
		if(arguments[i][0] == '$'){
			char *env = getenv(arguments[i] + 1);
			if(env) strcpy(arguments[i], env);
			else {printf("Environmental variable not found\n"); exit(EXIT_FAILURE);}
		}
		token = strtok(NULL, tokens);
		i++;
	}
	arguments[i] = NULL;
}