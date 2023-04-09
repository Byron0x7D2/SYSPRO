#pragma once


typedef struct CircularArray circulararray;
circulararray *circulararray_create_and_init();
void circulararray_destroy_and_save(circulararray *);
void circulararray_print(circulararray *);
void circulararray_add(circulararray *, char );
char *circulararray_get(circulararray *, int );
