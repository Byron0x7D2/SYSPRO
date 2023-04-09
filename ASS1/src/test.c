// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "../include/hash.h"
// #include "../include/defines.h"


// int main(){
// 	hash *h = hash_create_and_init();

// 	// hash_insert(h, "test", "test");
// 	// hash_insert(h, "test2", "test2");
// 	hash_insert(h, "test3", "\"cd /home/users/smith\"");
// 	hash_insert(h, "test4", "test4");
// 	// hash_insert(h, "test5", "test5");
// 	char *alias = hash_lookup(h, "test3");
// 	puts(alias);
// 	char *copy = malloc(sizeof(char) * MAX_WORD_LENGTH);
// 	strcpy(copy, alias);
// 	char *token = strtok(copy, "\" 	");
// 	while(token){ printf("%s\n", token); token = strtok(NULL, "\" 	");}
// 	free(copy);

// 	// printf("%s\n", hash_lookup(h, "test3"));
// 	hash_destroy_and_save(h);


// 	return 0;
// }