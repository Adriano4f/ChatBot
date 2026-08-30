#include "Hash_internal.h"

#include <assert.h>

size_t 
Hfind_slot
  (Hash *this, 
  const void *key,
  const  size_t size)
{
  /*
    Returns the slot holding the key, or the first slot available for it.
    Probing goes past tombstones, otherwise every key stored behind a deleted
    one becomes unreachable.
    Returns SIZE_MAX when no slot is available, it is up to the caller to
    resize the table and probe again.
  */
  if ( NULL == this || NULL == this->TABLE || NULL == key )
    return SIZE_MAX;

  assert((this->CAPACITY & (this->CAPACITY - 1)) == 0 && "CAPACITY must be power of two, what did you do");
  const size_t o_idx = hashfn(key, size) & (this->CAPACITY-1); // Mod operation is really slow, and if I don't micro optimise I get stressed
  size_t idx = o_idx;
  size_t free_slot = SIZE_MAX;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    idx %= this->CAPACITY;
    const EntryState state  = this->TABLE[idx].state;
    const RHEntry entry  = this->TABLE[idx];

    if ( EMPTY == state )
    {
      if ( SIZE_MAX == free_slot )
        free_slot = idx;
      break;
    }
    else if ( TOMBSTONE == state )
    {
      if ( SIZE_MAX == free_slot )
        free_slot = idx;
    }
    else if ( Hcompare_key_entry( key, size, entry ) )
      return idx;

    ++idx;
  }

  return free_slot;
}


bool 
Hcompare_key_entry
  (const void *key1,
  const size_t size,
  const RHEntry entry)
{
  if ( size != entry.ksize )
    return 0;
  const char *ckey1 = (const char *)key1;
  const char *ckey2 = (const char *)entry.key;

  for ( size_t i = 0; i < size; ++i )
  {
    if ( ckey1[i] != ckey2[i] )
      return 0;
  }
  return 1;
}


int 
Hrehash
  (Hash *this,
  RHEntry *TABLE,
  const size_t capacity)
{
  if ( NULL == this || NULL == TABLE )
    return H_INVALID_ARG;

  int err = H_SUCESS;
  for ( size_t i = 0; i < capacity; ++i )
  {
    if ( OCCUPIED != TABLE[i].state)
      continue;
    err = INSERT(this, TABLE[i].key, TABLE[i].ksize, TABLE[i].value, TABLE[i].vsize);
    if ( err )
      break;
  }
  return err;
}
