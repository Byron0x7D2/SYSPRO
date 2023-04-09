#pragma once

typedef struct Hash hash;

hash *hash_create_and_init();
char *hash_lookup(hash *, char *);
void hash_insert(hash *, char *, char *);
void hash_delete(hash *, char *);
void hash_destroy(hash *);
void hash_destroy_and_save(hash *);