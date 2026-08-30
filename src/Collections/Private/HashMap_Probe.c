#include "HashMap_internal.h"

#include <string.h>

#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL


uint64_t
HashBytes
  (const void *data,
  size_t size)
{
  const unsigned char *bytes = (const unsigned char *)data;
  uint64_t hash = FNV_OFFSET_BASIS;

  for ( size_t i = 0; i < size; ++i )
  {
    hash ^= bytes[i];
    hash *= FNV_PRIME;
  }

  return hash;
}


size_t
HashMapFindSlot
  (const HashMap *map,
  const void *key,
  size_t key_size,
  bool *found)
{
  size_t mask = map->capacity - 1;
  size_t index = (size_t)HashBytes(key, key_size) & mask;
  size_t first_tombstone = map->capacity; // capacity means "none seen"

  /*
    Probing runs past tombstones instead of stopping at them, otherwise a key
    inserted behind a deleted one becomes unreachable. The first tombstone is
    remembered so an insertion can reuse it.
  */
  for ( size_t probe = 0; probe < map->capacity; ++probe )
  {
    const Slot *slot = &map->slots[index];

    switch ( slot->state )
    {
      case SLOT_EMPTY:
        *found = false;
        return map->capacity == first_tombstone ? index : first_tombstone;

      case SLOT_TOMBSTONE:
        if ( map->capacity == first_tombstone )
          first_tombstone = index;
        break;

      case SLOT_OCCUPIED:
        if ( slot->key_size == key_size
             && 0 == memcmp(slot->key, key, key_size) )
        {
          *found = true;
          return index;
        }
        break;
    }

    index = (index + 1) & mask;
  }

  *found = false;
  return first_tombstone; // Full table: only a tombstone can be reused
}
