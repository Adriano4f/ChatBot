#ifndef DANA_HASHMAP_H
#define DANA_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
  Open-addressed hash map with linear probing and tombstones.

  Keys are arbitrary byte strings and values are fixed-size blobs, both copied
  into the map on insertion and freed by HashMapDestroy: nothing the caller
  passes in has to outlive the call.
*/

typedef struct HashMap HashMap;

// value_size is the size of every value stored in this map, and must not be 0
HashMap *
HashMapCreate
  (size_t value_size);

void
HashMapDestroy
  (HashMap *map);

size_t
HashMapCount
  (const HashMap *map);

/*
  Copies key and value into the map, replacing the value of an existing key.
  Returns a pointer to the stored value, which stays valid until the next
  insertion (a rehash moves it), or NULL on allocation failure.
  `value` may be NULL to leave the stored value zeroed on a new key,
  or unchanged on an existing one.
*/
void *
HashMapPut
  (HashMap *map,
  const void *key,
  size_t key_size,
  const void *value);

// Returns a pointer to the stored value, or NULL if the key is absent
void *
HashMapGet
  (const HashMap *map,
  const void *key,
  size_t key_size);

// Returns false if the key was not present
bool
HashMapRemove
  (HashMap *map,
  const void *key,
  size_t key_size);

typedef struct HashMapEntry
{
  const void *key;
  size_t      key_size;
  void       *value;
} HashMapEntry;

/*
  Iterates over every entry in an unspecified order. Start with cursor = 0 and
  do not modify the map while iterating.

    size_t cursor = 0;
    HashMapEntry entry;
    while ( HashMapNext(map, &cursor, &entry) )
      ...
*/
bool
HashMapNext
  (const HashMap *map,
  size_t *cursor,
  HashMapEntry *out);

uint64_t
HashBytes
  (const void *data,
  size_t size);

#endif
