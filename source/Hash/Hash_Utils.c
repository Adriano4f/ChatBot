#include "Hash_internal.h"

#include <assert.h>
#include <setjmp.h> // Satanic things

size_t 
Hfind_slot
  (Hash *this, 
  const void *key,
  const  size_t size)
{
  volatile int retried = 0;
  jmp_buf err;
  if ( setjmp(err) )
  {
    // A full table is grown once; failing to grow it is reported to the caller
    if ( retried || Hresize(this) )
      return SIZE_MAX;
    retried = 1;
  }

  assert((this->CAPACITY & (this->CAPACITY - 1)) == 0 && "CAPACITY must be power of two, what did you do");
  const size_t o_idx = hashfn(key, size) & (this->CAPACITY-1); // Mod operation is really slow, and if I don't micro optimise I get stressed
  size_t idx = o_idx;
  size_t toret = SIZE_MAX;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    idx %= this->CAPACITY;
    const EntryState state  = this->TABLE[idx].state;
    const RHEntry entry  = this->TABLE[idx];

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
  }
  if (SIZE_MAX == toret)
    longjmp(err, 1); // TODO: change this longjmp, worst thing in this file
  
  return toret;
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
  int nsuccess = 0;
  for ( size_t i = 0; i < capacity; ++i )
  {
    if ( OCCUPIED != TABLE[i].state)
      continue;
    nsuccess = INSERT(this, TABLE[i].key, TABLE[i].ksize, TABLE[i].value, TABLE[i].vsize);
    if ( nsuccess )
      break;
  }
  return nsuccess;
}
