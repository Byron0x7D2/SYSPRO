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
#include "../include/execute.h"
#include <glob.h>
#include "../include/hash.h"
#include "../include/circulararray.h"

/* Allocate memory for commands */
void get_memory(char ***arguments, char **srcfile, char **destfile, char **word){

	*srcfile = malloc(sizeof(char) * MAX_FILE_LENGTH);
	strcpy(*srcfile, "");

	*destfile = malloc(sizeof(char) * MAX_FILE_LENGTH);
	strcpy(*destfile, "");

	*word = malloc(sizeof(char) * MAX_WORD_LENGTH);

	*arguments = malloc(sizeof(char*) * MAX_ARGUMENTS);
	for(int i = 0; i < MAX_ARGUMENTS; i++){
		(*arguments)[i] = NULL;
	}

	if(!srcfile || !destfile || !word || !arguments){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
}

/* Free memory for commands */
void free_memory(char **arguments, char *srcfile, char *destfile, char *word){

	if(srcfile) free(srcfile);

	if(destfile) free(destfile);

	if(word) free(word);

	if(arguments){
		for(int i = 0; i < MAX_ARGUMENTS; i++){
			if(arguments[i]) free(arguments[i]);
			else break;
		}
		free(arguments);
	}
}

/* Stores character in word temp string */
int store_char(char *word, int *characters, char c){

	if(*characters < MAX_WORD_LENGTH){
		word[*characters] = c; 
		(*characters)++;
		return 1;
	}
	else{
		fprintf(stderr, "Too big word");
		return 0;
	}
}


/* Reads from file pointer untill it finds a word or a special character 
and returns to the caller function its findings */
int get_input_token(char *word, circulararray *ca, FILE *fp){

	char c;
	int state = NEUTRAL, characters = 0;

	while((c = fgetc(fp)) != EOF){

		if(state == NEUTRAL){

			circulararray_add(ca, c); // Add character to circular array for history keeping

			if(c == ' ' || c == '\t') continue;

			if(c == ';' || c == '&' || c == '|' || c == '<' || c == '\n') return c;
				
			if(c == '>'){
				
				c = fgetc(fp); // Check if next character is also >

				if(c == '>'){
					circulararray_add(ca, c);
					return GTGT;
				}else{
					ungetc(c, fp);
					return GT;
				}
			}

			if(c == '$') { 
				state = INDOLLAR;
				continue;
			}

			if(c == '\"'){
				state = INQUOTE;
				continue;
			}

			state = INWORD;
			if(!store_char(word, &characters, c))return MYERROR; // keep the character
			continue;

		}else if(state == INWORD){ // Word state, found a word and saving it whole

			if(c != ' ' && c != '\t' && c != ';' && c != '&' && c != '|' && c != '<' && c != '>' && c != '\n'){
				if(!store_char(word, &characters, c))return MYERROR;
				circulararray_add(ca, c);
				continue;
			}else{
				ungetc(c, fp);
				if(!store_char(word, &characters, '\0'))return MYERROR;
				return WORD;
			}

		}else if(state ==  INDOLLAR){ // Dollar state, found a $ and saving the environment variable

			if(c != ' ' && c != '\t' && c != ';' && c != '&' && c != '|' && c != '<' && c != '>' && c != '\n'){
				if(!store_char(word, &characters, c))return MYERROR;
				circulararray_add(ca, c);
				continue;
			}else{
				ungetc(c, fp);
				if(!store_char(word, &characters, '\0'))return MYERROR;
				char *env = getenv(word); // Get environment variable
				if(!env){
					fprintf(stderr, "No such environment variable");
					return MYERROR;
				}
				word[0] = '\0';
				strcpy(word, env);
				return WORD;
			}

		}else if(state ==  INQUOTE){ // Quote state, found a " and saving the word inside the quotes

			circulararray_add(ca, c);
			if(c != '\"'){
				if(!store_char(word, &characters, c))return MYERROR;
				continue;
			}else{
				if(!store_char(word, &characters, '\0'))return MYERROR;
				return WORD;
			}
		}
	}
	return MYERROR;
}

/* Checks if word has wildcards and if it does it expands them */
int wildcards(char *word, int *argc, char **argv){

	int i = 0;
	int check = 0;

	while(word[i] != '\0'){
		if(word[i] == '*' || word[i] == '?' || word[i] == '[' || word[i] == ']'){
			check =1; 	// Word has wildcards
			break;
		}
		i++;
	}

	if(check){ 

		glob_t globbuf;
		glob(word, 0, NULL, &globbuf);
		word[0] = '\0';
		for(int i = 0; i < globbuf.gl_pathc && i < MAX_WORD_LENGTH; i++){

			if(*argc >= MAX_ARGUMENTS) {
				fprintf(stderr, "Too many arguments");
				return 1;
			}
			argv[*argc] = malloc(sizeof(char) * MAX_WORD_LENGTH);
			if(!argv[*argc]){
				perror("malloc");
				return 0;
			}
			strcpy(argv[*argc], globbuf.gl_pathv[i]);
			(*argc)++;

		}
		
		globfree(&globbuf);
		return 1;
	}
	return 0;
}

/* Checks if word is an alias and if it is it expands it */
int aliases(char *word, hash *h){

	int i;
	char *alias = hash_lookup(h, word); // Check if word is an alias

	if(!alias){ // Word is not an alias
		return 0;
	}

	char *copy = malloc(sizeof(char) * MAX_WORD_LENGTH);
	if(!copy){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	strcpy(copy, alias); // Copy alias to copy string in order to not get destroyed by strtok
	char *token = strtok(copy, "\"");

	if(token){
		
		char temp[MAX_WORD_LENGTH];
		char buf[MAX_INPUT_LENGTH];  // write command to file so that it can be read by the main loop
		snprintf(buf, sizeof(buf), "%s/.newinput", getenv("HOME"));
		FILE *fptr = fopen(buf, "w+");
		if(!fptr){
			perror("fopen");
			return 0;
		}


		for(i = 0; token[i]; i++){
			temp[i] = token[i];
		}
		temp[i++] = '\n';
		temp[i] = '\0';

		fprintf(fptr, "%s", temp); // write command to file

		fclose(fptr);
		free(copy);
		return 1;

	}
	free(copy);
	return 0;
}

/* Main function for the inputs, basically each call is a command */
int command(int force_read, int force_write, int other_end, pid_t *wait_pid,  hash *h, circulararray *ca, FILE *fp){

	char **argv=NULL, *srcfile=NULL, *destfile=NULL, *word=NULL;
	get_memory(&argv, &srcfile, &destfile, &word);
	int token, argc = 0, append = 0; 
	int fd[2]; // Create pipe

	while(1){ // Loop until we get a command to execute

		token = get_input_token(word, ca, fp); // Get token

		if(token == WORD){

			if(!argc){ // If it is the first word of the command check if it is an alias
				if(aliases(word, h)){
					get_input_token(word, ca, fp); // Get next token to get rid of newline
					free_memory(argv, srcfile, destfile, word);

					*wait_pid = -2;
					return MYINPUT;
				}
				if(argc) continue;
			}

			if(wildcards(word, &argc, argv)) continue; // Check if word has wildcards and expand them

			if(argc >= MAX_ARGUMENTS) {
				fprintf(stderr, "Too many arguments");
				free_memory(argv, srcfile, destfile, word);
				return MYERROR;
			}

			argv[argc] = malloc(sizeof(char) * MAX_WORD_LENGTH); // Save word to argv

			if(!argv[argc]){
				perror("malloc");
				exit(EXIT_FAILURE);
			}	

			strcpy(argv[argc], word);
			argc++;

		}

		if(token == SEMI || token == AMP){

			if(argc != 0){
				argv[argc] = NULL;
				*wait_pid = execute(argv, srcfile, destfile, append, force_read, force_write, other_end, h, ca);
				if(token == AMP) *wait_pid = -1;
			}

			free_memory(argv, srcfile, destfile, word);

			return token; // this will make sure main won't print and will go to the next command from the same input
		}
		
		if(token == BAR){

			if(pipe(fd) == -1){
				perror("pipe");
				exit(EXIT_FAILURE);
			}

			argv[argc] = NULL;
			*wait_pid = execute(argv, srcfile, destfile, append, force_read, fd[WRITE], fd[READ], h, ca);

			argc = 0;
			free_memory(argv, srcfile, destfile, word);

			return command(fd[READ],-1, fd[WRITE], wait_pid,  h, ca, fp); // Call command again to get the next command with input the output of the previous command
		}

		if(token == LT){

			if(force_read != -1) { // Check if there is already an input redirect
				fprintf(stderr, "Ambiguous input redirect");
				free_memory(argv, srcfile, destfile, word);
				return MYERROR;
			}

			// Get file name
			while((token = get_input_token(word, ca, fp)) != WORD);

			strcpy(srcfile, word);

			continue;
		}

		if(token == GT || token == GTGT){

			if(token == GTGT) append = 1; 

			if(force_write != -1) {
				fprintf(stderr, "Ambiguous output redirect");
				free_memory(argv, srcfile, destfile, word);
				return MYERROR;
			}

			while((token = get_input_token(word, ca, fp)) != WORD);

			strcpy(destfile, word);

			continue;
		}

		if(token == NL){

			if(argc != 0){
				argv[argc] = NULL;

				if(!strcmp(argv[0], "rip")) { // Check if command is rip 
					free_memory(argv, srcfile, destfile, word);
					*wait_pid = -1;
					return MYEXIT;
				}

				*wait_pid = execute(argv, srcfile, destfile, append, force_read, force_write, other_end, h, ca);

				if(*wait_pid == -2){ // This means my history command was called and now we will change the input stream to the file
					free_memory(argv, srcfile, destfile, word);
					return MYINPUT;
				}

			}

			free_memory(argv, srcfile, destfile, word);
			return NL;
		}

		if(token == MYERROR){
			free_memory(argv, srcfile, destfile, word);
			return MYERROR;
		}
	}

	free_memory(argv, srcfile, destfile, word);
	return 0;
}

