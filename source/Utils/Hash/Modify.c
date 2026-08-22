#include "Utils/Hash/Internal.h"

int
Hinsert (Hash *self, const void *key, const size_t ksize,
         const void *value, const size_t vsize)
{
  const size_t idx = Hfind_slot(self, key, ksize);
  if ( (size_t)-1 == idx )
    return 1;

  if ( OCCUPIED != self->TABLE[idx].state )
    self->LOAD_FACTOR =  (++(self->SIZE))*1000/(self->CAPACITY);
  self->TABLE[idx] = (RHEntry){ (void *)key, ksize, (void *)value, vsize, (hashfn(key, ksize) & (self->CAPACITY-1)) - idx, OCCUPIED };
  

  if ( LF_RESIZE_TRIGGER_VALUE < self->LOAD_FACTOR && RESIZE(self) ) {}; // && Will only execute if first condition is met, self is to avoid warnings from the compiler
  if ( LF_THRESHOLD < self->LOAD_FACTOR )
    return RESIZE(self);
  
  return 0;
}

void
Hdelete (Hash *self, const void *key, const size_t size)
{
  /*
    When deleting a bucket or slot (making it available) only RHEntry.state and RHEntry.ksize
    state = TOMBSTONE; so it doesn't make inaccessible further keys
    ksize = 0; so in each comparison made with this key it is automatically ignored

    With that said, it is possible to conserve past values and access them manually.
  */
  const size_t idx = Hfind_slot(self, key, size);
  if ( (size_t)-1 != idx && Hcompare_key_entry( key, size, self->TABLE[idx] ) )
  {
    self->TABLE[idx].state = TOMBSTONE;
    self->TABLE[idx].ksize = 0;
  }

  return;
}
