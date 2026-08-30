#include "Hash_internal.h"

int
Hinsert
  (Hash *this,
  const void *key,
  const size_t ksize,
  const void *value,
  const size_t vsize)
{
  size_t idx = Hfind_slot(this, key, ksize);
  if ( SIZE_MAX == idx )
  {
    /* Nothing free: grow once and probe the new table. */
    if ( Hresize(this) )
      return 1;
    idx = Hfind_slot(this, key, ksize);
    if ( SIZE_MAX == idx )
      return 1;
  }

  if ( OCCUPIED != this->TABLE[idx].state )
    ++(this->SIZE);
  const size_t o_idx = hashfn(key, ksize) & (this->CAPACITY-1);
  this->TABLE[idx] = (RHEntry){ (void *)key, ksize, (void *)value, vsize, (int)((idx - o_idx) & (this->CAPACITY-1)), OCCUPIED };
  this->LOAD_FACTOR = this->SIZE*1000/(this->CAPACITY);

  if ( LF_RESIZE_TRIGGER_VALUE < this->LOAD_FACTOR )
    return Hresize(this);
  
  return 0;
}

void
Hdelete
  (Hash *this, 
  const void *key,
  const size_t size)
{
  /*
    When deleting a bucket or slot (making it available) only RHEntry.state and RHEntry.ksize
    state = TOMBSTONE; so it doesn't make inaccessible further keys
    ksize = 0; so in each comparison made with this key it is automatically ignored

    With this said, it is possible to conserve past values and access them manually.
  */
  const size_t idx = Hfind_slot(this, key, size);
  if ( SIZE_MAX != idx && Hcompare_key_entry( key, size, this->TABLE[idx] ) )
  {
    this->TABLE[idx].state = TOMBSTONE;
    this->TABLE[idx].ksize = 0;
    --(this->SIZE);
    this->LOAD_FACTOR = this->SIZE*1000/(this->CAPACITY);
  }

  return;
}
