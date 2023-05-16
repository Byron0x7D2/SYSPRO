#pragma once
typedef struct buffer Buffer;

Buffer *buffer_init(int );
int buffer_insert(Buffer *, int );
int buffer_remove(Buffer *);
int buffer_count(Buffer *);
int buffer_size(Buffer *);
void buffer_destroy(Buffer *);
