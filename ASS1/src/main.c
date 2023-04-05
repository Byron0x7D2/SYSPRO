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

int main(int argc, char *argv[]){
	char *input = NULL, *exe  = NULL, **arguments  = NULL;
	while(1){
		get_memory(&input, &exe, &arguments);
		printf("in-mysh-now:>");
		fgets(input, MAX_INPUT_LENGTH, stdin);
		parse_string(input, exe, arguments);
		if(!strcmp(exe, "rip")) {free_memory(input, exe, arguments); break;}
		
		execute(exe, arguments);
		free_memory(input, exe, arguments);

	}

	return 0;
}