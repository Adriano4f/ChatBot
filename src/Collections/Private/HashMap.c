#include "HashMap_internal.h"

#include "Core.h"

#include <stdlib.h>
#include <string.h>


HashMap *
HashMapCreate
  (size_t value_size)
{
  if ( 0 == value_size )
    return NULL;

  HashMap *map = (HashMap *)DanaAlloc(sizeof(HashMap), "HashMapCreate");
  if ( NULL == map )
    return NULL;

  map->slots = (Slot *)DanaCalloc(HASHMAP_INITIAL_CAPACITY, sizeof(Slot),
                                  "HashMapCreate");
  if ( NULL == map->slots )
  {
    free(map);
    return NULL;
  }

  map->capacity = HASHMAP_INITIAL_CAPACITY;
  map->count = 0;
  map->used = 0;
  map->value_size = value_size;

  return map;
}


void
HashMapDestroy
  (HashMap *map)
{
  if ( NULL == map )
    return;

  for ( size_t i = 0; i < map->capacity; ++i )
  {
    free(map->slots[i].key);
    free(map->slots[i].value);
  }

  free(map->slots);
  free(map);
}


size_t
HashMapCount
  (const HashMap *map)
{
  return NULL == map ? 0 : map->count;
}


int
HashMapGrow
  (HashMap *map)
{
  size_t capacity = map->capacity * 2;
  Slot *slots = (Slot *)DanaCalloc(capacity, sizeof(Slot), "HashMapGrow");
  if ( NULL == slots )
    return DANA_ENOMEM;

  Slot *old_slots = map->slots;
  size_t old_capacity = map->capacity;

  map->slots = slots;
  map->capacity = capacity;
  map->used = map->count;

  // Rehashing moves the existing copies, it never allocates again
  size_t mask = capacity - 1;
  for ( size_t i = 0; i < old_capacity; ++i )
  {
    if ( SLOT_OCCUPIED != old_slots[i].state )
    {
      free(old_slots[i].key);
      free(old_slots[i].value);
      continue;
    }

    size_t index = (size_t)HashBytes(old_slots[i].key, old_slots[i].key_size) & mask;
    while ( SLOT_EMPTY != slots[index].state )
      index = (index + 1) & mask;

    slots[index] = old_slots[i];
  }

  free(old_slots);
  return DANA_OK;
}


void *
HashMapPut
  (HashMap *map,
  const void *key,
  size_t key_size,
  const void *value)
{
  if ( NULL == map || NULL == key || 0 == key_size )
    return NULL;

  if ( (map->used + 1) * HASHMAP_MAX_LOAD_DEN
       > map->capacity * HASHMAP_MAX_LOAD_NUM
       && DANA_OK != HashMapGrow(map) )
    return NULL;

  bool found = false;
  size_t index = HashMapFindSlot(map, key, key_size, &found);
  Slot *slot = &map->slots[index];

  if ( found )
  {
    if ( NULL != value )
      memcpy(slot->value, value, map->value_size);
    return slot->value;
  }

  void *key_copy = DanaAlloc(key_size, "HashMapPut");
  if ( NULL == key_copy )
    return NULL;

  void *value_copy = DanaCalloc(1, map->value_size, "HashMapPut");
  if ( NULL == value_copy )
  {
    free(key_copy);
    return NULL;
  }
  if ( NULL != value )
    memcpy(value_copy, value, map->value_size);

  memcpy(key_copy, key, key_size);

  if ( SLOT_EMPTY == slot->state )
    ++map->used; // Reusing a tombstone does not change the load

  free(slot->key); // A tombstone still holds the copies of the removed entry
  free(slot->value);

  slot->key = key_copy;
  slot->key_size = key_size;
  slot->value = value_copy;
  slot->state = SLOT_OCCUPIED;
  ++map->count;

  return slot->value;
}


void *
HashMapGet
  (const HashMap *map,
  const void *key,
  size_t key_size)
{
  if ( NULL == map || NULL == key || 0 == key_size )
    return NULL;

  bool found = false;
  size_t index = HashMapFindSlot(map, key, key_size, &found);

  return found ? map->slots[index].value : NULL;
}


bool
HashMapRemove
  (HashMap *map,
  const void *key,
  size_t key_size)
{
  if ( NULL == map || NULL == key || 0 == key_size )
    return false;

  bool found = false;
  size_t index = HashMapFindSlot(map, key, key_size, &found);
  if ( !found )
    return false;

  Slot *slot = &map->slots[index];
  free(slot->key);
  free(slot->value);
  slot->key = NULL;
  slot->key_size = 0;
  slot->value = NULL;
  slot->state = SLOT_TOMBSTONE;
  --map->count;

  return true;
}


bool
HashMapNext
  (const HashMap *map,
  size_t *cursor,
  HashMapEntry *out)
{
  if ( NULL == map || NULL == cursor || NULL == out )
    return false;

  for ( size_t i = *cursor; i < map->capacity; ++i )
  {
    if ( SLOT_OCCUPIED != map->slots[i].state )
      continue;

    out->key = map->slots[i].key;
    out->key_size = map->slots[i].key_size;
    out->value = map->slots[i].value;
    *cursor = i + 1;
    return true;
  }

  *cursor = map->capacity;
  return false;
}
