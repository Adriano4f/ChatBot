#include "Utils/Hash/Internal.h"

#include <assert.h>
#include <setjmp.h> // Satanic things

size_t
Hfind_slot (Hash *self, const void *key, const size_t size)
{
  jmp_buf err;
  if ( setjmp(err) )
  {
    RESIZE(self);
  }

  assert((self->CAPACITY & (self->CAPACITY - 1)) == 0 && "CAPACITY must be power of two, what did you do");
  const size_t o_idx = hashfn(key, size) & (self->CAPACITY-1); // hashfn mod capacity (with capacity being a pow of 2)
  size_t idx = o_idx;
  size_t toret = SIZE_MAX;

  for ( size_t i = 0; i < self->CAPACITY; ++i )
  {
    idx %= self->CAPACITY;
    const EntryState state  = self->TABLE[idx].state;
    const RHEntry entry  = self->TABLE[idx];

    if ( TOMBSTONE == state || EMPTY == state )
    {
      toret = idx;
      break;
    }
    else if ( Hcompare_key_entry( key, size, entry ) )
    {
      toret = idx;
      break;
    }
    ++idx;
    if (SIZE_MAX == toret)
    longjmp(err, 1); // TODO: change self longjmp, worst thing in self file
  }
  
  return toret;
}


bool
Hcompare_key_entry (const void *key1, const size_t size, const RHEntry entry)
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
Hrehash (Hash *self, RHEntry *TABLE)
{
  int nsuccess = 0;
  for ( size_t i = 0; i < (self->CAPACITY); ++i )
  {
    if ( OCCUPIED != TABLE[i].state)
      continue;
    nsuccess = INSERT(self, TABLE[i].key, TABLE[i].ksize, TABLE[i].value, TABLE[i].vsize);
    if ( nsuccess )
      break;
  }
  return nsuccess;
}
