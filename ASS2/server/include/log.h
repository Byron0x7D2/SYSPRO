#pragma once

typedef struct Hash hash;

hash *hash_create(char *);
int hash_lookup(hash *, char *);
void hash_insert(hash *, char *, char *);
void hash_destroy(hash *);
