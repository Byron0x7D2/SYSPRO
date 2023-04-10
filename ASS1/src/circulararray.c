#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/circulararray.h"
#include "../include/defines.h"

// used to be implemented with an array but now it is a linked list, oh well


struct CircularArray{
	int size;   // number of elements in the array
	struct Node *head; // pointer to the first element in the array
	char *temp; // temp buffer for storing input
	int index; // index of the temp buffer
};

struct Node{ // node of the linked list that would have been an array if it didn't annoy me
	char data[MAX_INPUT_LENGTH];
	struct Node *next;
};
typedef struct Node node;

/* Inserts string at the start of the list, 
if it surpasses the maximum items allowed to be kept in history it removes the oldest one */
void circulararray_insert_at_start(circulararray *ca, char *data){

	char *p = strstr(data, "myhistory"); // don't save myhistory command
	if(p) return;

	node *new_node = malloc(sizeof(node));
	strcpy(new_node->data, data);
	new_node->next = ca->head;
	ca->head = new_node;
	ca->size++;

	if(ca->size > HISTORY_SIZE){
		node *p = ca->head;
		while(p->next->next){
			p = p->next;
		}
		free(p->next);
		p->next = NULL;
		ca->size--;
	}

}


/* Memory allocation and loading of the data from the history file */
circulararray *circulararray_create_and_init(){

	circulararray *ca = malloc(sizeof(circulararray));
	ca->temp = malloc(sizeof(char) * MAX_INPUT_LENGTH);
	ca->size = 0;
	ca->head = NULL;
	ca->index = 0;

	FILE *fptr;
	int entries;
	char c;
	char buf[MAX_INPUT_LENGTH];
	snprintf(buf, sizeof(buf), "%s/.history", getenv("HOME"));
	fptr = fopen(buf, "r");
	if(!fptr){
		return ca;
	}

	if(fscanf(fptr, "%d ", &entries) > 0){
		for(int i = 0; i < entries; i++){
			while((c = fgetc(fptr)) != '\n'){
				ca->temp[ca->index++] = c;	
			}
			ca->temp[ca->index++] = '\n';
			ca->temp[ca->index] = '\0';
			ca->index = 0;
			circulararray_insert_at_start(ca, ca->temp);
		}
	}
	fclose(fptr);
	return ca;
}

/* Deallocates memory allocated for this ADT, 
also saves the history to a file for future use */
void circulararray_destroy_and_save(circulararray *ca){

	FILE *fptr;
	char buf[MAX_INPUT_LENGTH];
	snprintf(buf, sizeof(buf), "%s/.history", getenv("HOME"));
	fptr = fopen(buf, "w");
	if(!fptr){
		return;
	}

	fprintf(fptr, "%d\n", ca->size); // save the number of entries
	node *p = ca->head;
	while(p){
		fprintf(fptr, "%s", p->data);
		p = p->next;
	}
	fclose(fptr);

	p = ca->head;
	while(p){
		node *temp = p;
		p = p->next;
		free(temp);
	}
	free(ca->temp);
	free(ca);
}

/* Prints the history*/
void circulararray_print(circulararray *ca){
	int i = 0;
	node *p = ca->head;
	while(p){
		printf("%d: %s",i, p->data);
		p = p->next;
		i++;
	}
	
}

/* Gets a character and saves it to the buffer,
once a line is terminated, it saves it in the actual list */
void circulararray_add(circulararray *ca, char c){
	ca->temp[ca->index++] = c;
	if(c == '\n' || ca->index == MAX_INPUT_LENGTH-1){
		ca->temp[ca->index] = '\0';
		circulararray_insert_at_start(ca, ca->temp);
		ca->index = 0;
	}

}

/* Gets the string at the specified index */
char *circulararray_get(circulararray *ca, int index){
	int i = 0;
	node *p = ca->head;
	while(p){
		if(i == index){
			return p->data;
		}
		p = p->next;
		i++;
	}
	return NULL;
}