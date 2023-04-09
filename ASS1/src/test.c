#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/circulararray.h"
#include "../include/defines.h"


int main(){
	circulararray *ca = circulararray_create_and_init();
	char c;
	while((c = getc(stdin)) != EOF){
		circulararray_add(ca, c);
	}
	circulararray_print(ca);
	circulararray_destroy_and_save(ca);		
	return 0;
}