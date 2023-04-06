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
	int last_status = NL;
	while(1){		
		if(last_status == NL)printf("in-mysh-now:>");
		last_status = command();
		if(last_status == MYEXIT) break;
	}

	return 0;
}