#ifndef DANA_HASHMAP_INTERNAL_H
#define DANA_HASHMAP_INTERNAL_H

#include "HashMap.h"

#define HASHMAP_INITIAL_CAPACITY 8
#define HASHMAP_MAX_LOAD_NUM     7 // Grows when count + tombstones exceeds
#define HASHMAP_MAX_LOAD_DEN     10 // capacity * 7/10

typedef enum SlotState
{
  SLOT_EMPTY,
  SLOT_OCCUPIED,
  SLOT_TOMBSTONE
} SlotState;

typedef struct Slot
{
  void     *key;
  size_t    key_size;
  void     *value;
  SlotState state;
} Slot;

struct HashMap
{
  Slot  *slots;
  size_t capacity; // Always a power of two
  size_t count;
  size_t used;     // Occupied + tombstoned slots, what the load factor is about
  size_t value_size;
};

/*
  Returns the index of the slot holding `key`, or of the slot it should be
  written to when absent. Sets *found.
*/
size_t
HashMapFindSlot
  (const HashMap *map,
  const void *key,
  size_t key_size,
  bool *found);

int
HashMapGrow
  (HashMap *map);

#endif
