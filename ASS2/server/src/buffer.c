# include <stdio.h>
# include <stdlib.h>
#include "../include/buffer.h"

/* Buffer struct */
struct buffer{
	int size;
	int start;
	int end;
	int count;
	int *array;
};

/* Initialize a buffer */
Buffer *buffer_init(int size){

	Buffer *b = malloc(sizeof(Buffer));
	b->size = size;
	b->start = 0;
	b->end = 0;
	b->count = 0;
	b->array = malloc(size*sizeof(int));
	return b;
}

/* Insert value to buffer */
int buffer_insert(Buffer *b, int value){

	if(b->count == b->size) return 0;

	b->array[b->end] = value;
	b->end = (b->end + 1) % b->size;
	b->count++;
	return 1;
}

/* Remove value from buffer */
int buffer_remove(Buffer *b){

	if(!b->count) return -1;

	int value = b->array[b->start];
	b->start = (b->start + 1) % b->size;
	b->count--;
	return value;
}

/* Return elements of buffer */
int buffer_count(Buffer *b){
	return b->count;
}

/* Return max size of buffer */
int buffer_size(Buffer *b){
	return b->size;
}

/* Destroy buffer */
void buffer_destroy(Buffer *b){
	free(b->array);
	free(b);
}

/* Tester */
// int main(){
// 	Buffer *b = buffer_init(10);
// 	for(int i = 0; i < 10; i++){
// 		buffer_insert(b, i);
// 	}
// 	for(int i = 0; i < 6; i++){
// 		printf("%d\n", buffer_remove(b));
// 	}
// 	for(int i = 0; i < 6; i++){
// 		buffer_insert(b, i+10);
// 	}
// 	for(int i = 0; i < 10; i++){
// 		printf("%d\n", buffer_remove(b));
// 	}
// 	buffer_destroy(b);
// }