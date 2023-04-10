#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash.h"
#include "../include/defines.h"

int hashfun(char *key){
	int sum = 0;
	for(int i = 0; i < strlen(key); i++){
		sum += key[i];
	}
	return sum % HASH_SIZE;
}

struct Entry{
	char key[MAX_WORD_LENGTH];
	char value[MAX_INPUT_LENGTH];
	struct Entry *next;
};

typedef struct Entry entry;

struct Hash{
	entry **table;
	int entries;
};

hash *hash_create_and_init(){
	hash *h = malloc(sizeof(hash));
 	h->entries = 0;
	h->table = malloc(sizeof(entry*) * HASH_SIZE);
	for(int i = 0; i < HASH_SIZE; i++){
		h->table[i] = NULL;
	}

	FILE *fptr;
	int entries;
	char buf[MAX_INPUT_LENGTH];
	snprintf(buf, sizeof(buf), "%s/.aliases", getenv("HOME"));
	fptr = fopen(buf, "r");
	if(!fptr){
		return h;
	}
	if(fscanf(fptr, "%d ", &entries) > 0){
		for(int i = 0; i < entries; i++){
			char *key;
			char *value;
			key = malloc(sizeof(char) * MAX_WORD_LENGTH);
			value = malloc(sizeof(char) * MAX_INPUT_LENGTH);
			char c;
			int j = 0;
			while((c = fgetc(fptr)) != '\n'){
				key[j++] = c;	
			}
			key[j] = '\0';
			j = 0;
			while((c = fgetc(fptr)) != '\n'){
				value[j++] = c;	
			}
			value[j] = '\0';
			hash_insert(h, key, value);
			free(key);
			free(value);
		}
	}
	fclose(fptr);
	return h;
}

char *hash_lookup(hash *h, char *key){
	int index = hashfun(key);
	entry *p = h->table[index];
	while(p){
		if(strcmp(p->key, key) == 0){
			return p->value;
		}
		p = p->next;
	}
	return NULL;
}

void hash_delete(hash *h, char *key){
	int index = hashfun(key);
	entry *p = h->table[index];
	entry *prev = NULL;
	while(p){
		if(strcmp(p->key, key) == 0){
			if(prev) prev->next = p->next;
			else h->table[index] = p->next;
			h->entries--;
			free(p);
			return;
		}
		prev = p;
		p = p->next;
	}
}

void hash_insert(hash *h, char *key, char *value){
	if(hash_lookup(h,key)) return;
	h->entries++;
	int index = hashfun(key);
	entry *e = malloc(sizeof(entry));
	strcpy(e->key, key);
	strcpy(e->value, value);
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
}

void hash_destroy_and_save(hash *h){
	FILE *fptr;
	char buf[MAX_INPUT_LENGTH];
	snprintf(buf, sizeof(buf), "%s/.aliases", getenv("HOME"));
	fptr = fopen(buf, "w");
	if(!fptr){
		fprintf(stderr, "Error opening aliases file\n");
		exit(EXIT_FAILURE);
	}
	if(!h) return;
	fprintf(fptr, "%d\n", h->entries);
	for(int i = 0; i < HASH_SIZE; i++){
		entry *p = h->table[i];
		while(p){
			fprintf(fptr, "%s\n", p->key);
			fprintf(fptr, "%s\n", p->value);
			p = p->next;
		}
	}
	fclose(fptr);
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