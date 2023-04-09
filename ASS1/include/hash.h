#pragma once

typedef struct Hash hash;

hash *hash_create_and_init();
char *hash_lookup(hash *h, char *key);
void hash_insert(hash *h, char *key, char *value);
void hash_delete(hash *h, char *key);
void hash_destroy(hash *h);
void hash_destroy_and_save(hash *h);