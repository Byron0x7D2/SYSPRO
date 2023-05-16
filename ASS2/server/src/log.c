#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/log.h"
#include "../include/defines.h"

/* Simple hash function */
int hashfun(char *key){ 
	int sum = 0;
	for(int i = 0; i < strlen(key); i++){
		sum += key[i];
	}
	return sum % HASH_SIZE;
}

typedef struct votes Votes;

struct votes{
	char party[MAX_NAME_LENGTH];
	int votes;
	Votes *next;
};


struct Entry{ // node to be saved in the hash table, when we have conflicts we go to the next node
	char key[MAX_NAME_LENGTH];
	char votes[MAX_NAME_LENGTH];
	struct Entry *next;
};

typedef struct Entry entry;

struct Hash{	// hash table
	entry **table;
	char filename[100];
	int entries;
	Votes* vot;
};

hash *hash_create(char *filename){

	hash *h = malloc(sizeof(hash));
 	h->entries = 0;
	h->table = malloc(sizeof(entry*) * HASH_SIZE);
	for(int i = 0; i < HASH_SIZE; i++){
		h->table[i] = NULL;
	}
	strcpy(h->filename, filename);
	h->vot = NULL;
	return h;
}


int hash_lookup(hash *h, char *key){

	int index = hashfun(key);
	entry *p = h->table[index];

	while(p){
		if(strcmp(p->key, key) == 0){
			return 1;
		}
		p = p->next;
	}
	return 0;
}



/* Inserts a new entry in the hash table */
void hash_insert(hash *h, char *key, char* value){

	h->entries++;
	int index = hashfun(key);

	entry *e = malloc(sizeof(entry));
	strcpy(e->key, key);
	strcpy(e->votes, value);
	
	entry *p = h->table[index];

	if(!p){
		h->table[index] = e;
		e->next = NULL;
	}else{
		while(p->next){
			p = p->next;
		}
		p->next = e;
		e->next = NULL;
	}

	// print to file
	FILE *fp = fopen(h->filename, "a");
	fprintf(fp, "%s %s\n", key, value);
	fclose(fp);

	// go through the votes list and if it finds the party, it increases the votes or creates a new node

	Votes *v = h->vot;
	if(!v){
		h->vot = malloc(sizeof(Votes));
		strcpy(h->vot->party, value);
		h->vot->votes = 1;
		h->vot->next = NULL;
	}else{
		while(v){
			if(strcmp(v->party, value) == 0){
				v->votes++;
				break;
			}
			if(!v->next){
				v->next = malloc(sizeof(Votes));
				strcpy(v->next->party, value);
				v->next->votes = 1;
				v->next->next = NULL;
				break;
			}
			v = v->next;
		}
	}
	

}

void poll_stats_fun(hash *h, char *filename){
	
	FILE *fp = fopen(filename, "w");
	// for eacg party in the votes list, it prints the party and the number of votes
	Votes *v = h->vot;
	while(v){
		fprintf(fp, "%s	%d\n", v->party, v->votes);
		v = v->next;
	}

	fclose(fp);
}


/* Frees the memory allocated for the hash table
and saves the aliases in the memory file */
void hash_destroy(hash *h){

	Votes *v = h->vot;
	while(v){
		Votes *temp = v;
		v = v->next;
		free(temp);
	}

	
	for(int i = 0; i < HASH_SIZE; i++){
		entry *p = h->table[i];
		while(p){
			entry *temp = p;
			p = p->next;
			free(temp);
		}
	}
	free(h->table);
	free(h);
}


