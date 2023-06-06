#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/defines.h"
#include "../include/list.h"




struct pending{
	char name[MAX_NAME_LENGTH];
	Pending *next;
};

int insert_if_not_inside(Pending **head, char* name){
	Pending *temp = *head;
	while(temp){
		if(strcmp(temp->name, name) == 0){
			return 0;
		}
		temp = temp->next;
	}
	Pending *new = malloc(sizeof(Pending));
	strcpy(new->name, name);
	new->next = *head;
	*head = new;
	return 1;
}

Pending* remove_from_pending(Pending *head, char* name){
	Pending *temp = head;
	Pending *prev = NULL;
	while(temp){
		if(strcmp(temp->name, name) == 0){
			if(!prev){
				head = temp->next;
			}else{
				prev->next = temp->next;
			}
			free(temp);
			return head;
		}
		prev = temp;
		temp = temp->next;
	}
	return head;
}