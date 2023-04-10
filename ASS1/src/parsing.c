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



int get_input_token(char *word, circulararray *ca, FILE *fp){
	char c;
	int state = NEUTRAL, characters = 0;

	while((c = fgetc(fp)) != EOF){
		switch (state){
			case NEUTRAL:
				circulararray_add(ca, c);
				switch (c){
					case ';':
						return SEMI;
					case '&':
						return AMP;
					case '|':
						return BAR;
					case '<':
						return LT;
					case '>':
						c = fgetc(fp);
						switch (c){
							case '>':
								circulararray_add(ca, c);
								return GTGT;
							default:
								ungetc(c, fp);
								return GT;
						}
					case ' ':
						continue;
					case '\t':
						continue;
					case '\n':
						return NL;
					case '$':
						state = INDOLLAR;
						continue;
					case '\"':
						state = INQUOTE;
						continue;
					default:
						state = INWORD;
						if(!store_char(word, &characters, c))return MYERROR;
						continue;
				}
			case INWORD:
				if(c != ' ' && c != '\t' && c != ';' && c != '&' && c != '|' && c != '<' && c != '>' && c != '\n'){
					if(!store_char(word, &characters, c))return MYERROR;
					circulararray_add(ca, c);
					continue;
				}
				else{
					ungetc(c, fp);
					if(!store_char(word, &characters, '\0'))return MYERROR;
					return WORD;
				}
			case INDOLLAR:
				if(c != ' ' && c != '\t' && c != ';' && c != '&' && c != '|' && c != '<' && c != '>' && c != '\n'){
					if(!store_char(word, &characters, c))return MYERROR;
					circulararray_add(ca, c);
					continue;
				}
				else{
					ungetc(c, fp);
					if(!store_char(word, &characters, '\0'))return MYERROR;
					char *env = getenv(word);
					if(!env){
						fprintf(stderr, "No such environment variable");
						return MYERROR;
					}
					word[0] = '\0';
					strcpy(word, env);
					return WORD;
				}
			case INQUOTE:
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

void wildcards(char *word){
	int i = 0;
	int check = 0;
	while(word[i] != '\0'){
		if(word[i] == '*' || word[i] == '?' || word[i] == '[' || word[i] == ']'){
			check =1;
			break;
		}
		i++;
	}
	if(check){
		glob_t globbuf;
		glob(word, 0, NULL, &globbuf);
		word[0] = '\0';
		for(int i = 0; i < globbuf.gl_pathc; i++){
			strcat(word, globbuf.gl_pathv[i]);
			strcat(word, " ");
		}
		globfree(&globbuf);
	}
}

void aliases(char *word, hash *h, int *argc, char **argv){
	char *alias = hash_lookup(h, word);
	if(!alias){
		return ;
	}
	char *copy = malloc(sizeof(char) * MAX_WORD_LENGTH);
	strcpy(copy, alias);
	char *token = strtok(copy, "\" 	");
	while(token){
		if(*argc >= MAX_ARGUMENTS) {
			fprintf(stderr, "Too many arguments");
			return ;
		}
		argv[*argc] = malloc(sizeof(char) * MAX_WORD_LENGTH);
		if(!argv[*argc]){
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		strcpy(argv[*argc], token);
		(*argc)++;
		token = strtok(NULL, "\" 	");
	}
	free(copy);
}

int command(int force_read, int force_write, int other_end, pid_t *wait_pid, hash *h, circulararray *ca, FILE *fp){
	char **argv=NULL, *srcfile=NULL, *destfile=NULL, *word=NULL;
	get_memory(&argv, &srcfile, &destfile, &word);
	int token, argc = 0, append = 0; 
	while(1){
		switch(token = get_input_token(word, ca, fp)){
			case WORD:

				if(!argc){
					aliases(word, h, &argc, argv);
					if(argc) break;
				}

				wildcards(word);

				if(argc >= MAX_ARGUMENTS) {
					fprintf(stderr, "Too many arguments");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				argv[argc] = malloc(sizeof(char) * MAX_WORD_LENGTH);
				if(!argv[argc]){
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				strcpy(argv[argc], word);
				argc++;
				break;
			case SEMI:
				if(argc != 0){
					argv[argc] = NULL;
					*wait_pid = execute(argv, srcfile, destfile, append, force_read, force_write, other_end, h, ca);
				}
				free_memory(argv, srcfile, destfile, word);
				return SEMI;
			case AMP:
				if(argc != 0){
					argv[argc] = NULL;
					*wait_pid = execute(argv, srcfile, destfile, append, force_read, force_write, other_end, h, ca);
				}
				free_memory(argv, srcfile, destfile, word);
				return AMP;
			case BAR:
				int fd[2];
				if(pipe(fd) == -1){
					perror("pipe");
					exit(EXIT_FAILURE);
				}
				argv[argc] = NULL;
				int bpid, wpid, status;
				bpid = execute(argv, srcfile, destfile, append, force_read, fd[WRITE], fd[READ], h, ca);
				if(bpid > 0){
					do{
						wpid = waitpid(-1, &status, 0);
					}while(wpid != bpid);
				}
				argc = 0;
				free_memory(argv, srcfile, destfile, word);
				return command(fd[READ],-1, fd[WRITE], wait_pid, h, ca, fp);
			case LT:
				if(force_read != -1) {
					fprintf(stderr, "Ambiguous input redirect");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				if((token = get_input_token(word, ca, fp)) != WORD){
					fprintf(stderr, "No file name");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				strcpy(srcfile, word);
				continue;
			case GT:
				if(force_write != -1) {
					fprintf(stderr, "Ambiguous output redirect");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				if((token = get_input_token(word, ca, fp)) != WORD){
					fprintf(stderr, "No file name");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				strcpy(destfile, word);
				continue;
			case GTGT:
				if(force_write != -1) {
					fprintf(stderr, "Ambiguous output redirect");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				if((token = get_input_token(word, ca, fp)) != WORD){
					fprintf(stderr, "No file name");
					free_memory(argv, srcfile, destfile, word);
					return MYERROR;
				}
				strcpy(destfile, word);
				append = 1;
				continue;
			case NL:
				if(argc != 0){
					argv[argc] = NULL;
					if(!strcmp(argv[0], "rip")) {
						free_memory(argv, srcfile, destfile, word);
						*wait_pid = -1;
						return MYEXIT;
					}
					*wait_pid = execute(argv, srcfile, destfile, append, force_read, force_write, other_end, h, ca);
					if(*wait_pid == -2){
						free_memory(argv, srcfile, destfile, word);
						return MYINPUT;
					}
				}
				free_memory(argv, srcfile, destfile, word);
				return NL;
			case MYERROR:
				free_memory(argv, srcfile, destfile, word);
				return MYERROR;
		}
	}

	free_memory(argv, srcfile, destfile, word);
	return 0;
}

