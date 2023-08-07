/*
 *
 * Copyright 2023 Doğu Kocatepe
 * This file is part of Theory Lisp.

 * Theory Lisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Theory Lisp is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.

 * You should have received a copy of the GNU General Public License along
 * with Theory Lisp. If not, see <https://www.gnu.org/licenses/>.
 */

#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

#include "list.h"

const double MAX_LOAD_FACTOR = 0.75;

typedef struct hashtable_pair {
  char *key;
  void *value;
} hashtable_pair_t;

typedef struct hashtable {
  listptr *data;
  size_t capacity;
  size_t number_of_pairs;
} hashtable_t;

static size_t hash(const char *key) {
  const size_t P1 = 127;
  const size_t P2 = 3457;
  const size_t P3 = 7919;

  size_t result = P1;
  for (size_t i = 0; i < strlen(key); ++i) {
    result = (P2 * result) ^ (P3 * key[i]);
  }

  return result;
}

hashtableptr new_hash_table(size_t capacity) {
  hashtableptr ht = malloc(sizeof *ht);
  ht->capacity = capacity;
  ht->number_of_pairs = 0;
  ht->data = calloc(ht->capacity, sizeof(listptr));
  return ht;
}

void delete_hash_table(hashtableptr table, dict_value_destructor destr) {
  for (size_t i = 0; i < table->capacity; ++i) {
    listptr lst = table->data[i];
    if (lst != NULL) {
      for (size_t j = 0; j < list_size(lst); ++j) {
        hashtable_pair_t *pair = list_get(lst, j);
        free(pair->key);
        destr(pair->value);
        free(pair);
      }
      delete_list(table->data[i]);
    }
  }

  free(table->data);
  table->capacity = 0;
  table->number_of_pairs = 0;
  free(table);
}

static void hash_table_rehash(hashtableptr table) {
  hashtableptr new_dict = new_hash_table(table->capacity * 2);
  for (size_t i = 0; i < table->capacity; ++i) {
    listptr lst = table->data[i];
    if (lst != NULL) {
      for (size_t j = 0; j < list_size(lst); ++j) {
        hashtable_pair_t *pair = list_get(lst, j);
        hash_table_put(new_dict, pair->key, pair->value);
        free(pair->key);
        free(pair);
      }

      delete_list(lst);
    }
  }

  free(table->data);

  table->data = new_dict->data;
  table->capacity = new_dict->capacity;
  table->number_of_pairs = new_dict->number_of_pairs;

  free(new_dict);
}

void *hash_table_put(hashtableptr table, const char *key, void *value) {
  double load_factor = (double)table->number_of_pairs / (double)table->capacity;
  if (load_factor >= MAX_LOAD_FACTOR) {
    hash_table_rehash(table);
  }

  size_t index = hash(key) % table->capacity;
  if (!table->data[index]) {
    hashtable_pair_t *new_pair = malloc(sizeof *new_pair);
    new_pair->key = strdup(key);
    new_pair->value = value;
    list_add(table->data[index] = new_list(), new_pair);
  } else {
    listptr lst = table->data[index];
    for (size_t i = 0; i < list_size(lst); ++i) {
      hashtable_pair_t *pair = list_get(lst, i);
      if (strcmp(pair->key, key) == 0) {
        void *result = pair->value;
        pair->value = value;
        ++table->number_of_pairs;
        return result;
      }
    }

    hashtable_pair_t *new_pair = malloc(sizeof *new_pair);
    new_pair->key = strdup(key);
    new_pair->value = value;
    list_add(lst, new_pair);
  }

  ++table->number_of_pairs;
  return NULL;
}

void *hash_table_get(hashtableptr table, const char *key) {
  size_t index = hash(key) % table->capacity;
  listptr lst = table->data[index];
  if (lst) {
    for (size_t i = 0; i < list_size(lst); ++i) {
      hashtable_pair_t *pair = list_get(lst, i);
      if (strcmp(pair->key, key) == 0) {
        return pair->value;
      }
    }
  }

  return NULL;
}
