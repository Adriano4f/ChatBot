#include "Hash_internal.h"

#include <assert.h>

/*
  Returns the slot the key lives in, or the slot it should be written to when it
  is not in the table yet. Tombstones do not stop the probe, they are only
  remembered as the best insertion candidate: a key inserted after a deletion
  can sit further down the chain, and stopping early would hide it.

  SIZE_MAX means every slot was probed without finding the key or a free slot,
  which only happens on a completely occupied table.
*/
size_t 
Hfind_slot
  (Hash *this, 
  const void *key,
  const  size_t size)
{
  assert((this->CAPACITY & (this->CAPACITY - 1)) == 0 && "CAPACITY must be power of two, what did you do");
  const size_t o_idx = hashfn(key, size) & (this->CAPACITY-1); // Mod operation is really slow, and if I don't micro optimise I get stressed
  size_t free_idx = SIZE_MAX;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    const size_t idx = (o_idx + i) & (this->CAPACITY-1);
    const RHEntry entry = this->TABLE[idx];

    if ( EMPTY == entry.state )
      return SIZE_MAX == free_idx ? idx : free_idx;

    if ( TOMBSTONE == entry.state )
    {
      if ( SIZE_MAX == free_idx )
        free_idx = idx;
      continue;
    }

    if ( Hcompare_key_entry( key, size, entry ) )
      return idx;
  }

  return free_idx;
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


/*
  Moves every entry of TABLE, which holds OLD_CAPACITY slots, into the already
  installed table of this.
*/
int 
Hrehash
  (Hash *this,
  RHEntry *TABLE,
  const size_t OLD_CAPACITY)
{
  int nsuccess = 0;
  for ( size_t i = 0; i < OLD_CAPACITY; ++i )
  {
    if ( OCCUPIED != TABLE[i].state)
      continue;
    nsuccess = INSERT(this, TABLE[i].key, TABLE[i].ksize, TABLE[i].value, TABLE[i].vsize);
    if ( nsuccess )
      break;
  }
  return nsuccess;
}
