#include "Hash_internal.h"

int
Hinsert
  (Hash *this,
  const void *key,
  const size_t ksize,
  const void *value,
  const size_t vsize)
{
  if ( NULL == this || NULL == this->TABLE || NULL == key )
    return H_INVALID_ARG;

  size_t idx = Hfind_slot(this, key, ksize);
  if ( SIZE_MAX == idx )
  {
    // No slot left, grow the table and probe again
    const int err_Hresize = Hresize(this);
    if ( err_Hresize )
      return err_Hresize;

    idx = Hfind_slot(this, key, ksize);
    if ( SIZE_MAX == idx )
      return H_TABLE_FULL;
  }

  if ( OCCUPIED != this->TABLE[idx].state )
    this->LOAD_FACTOR =  ++(this->SIZE)*1000/(this->CAPACITY);
  this->TABLE[idx] = (RHEntry){ (void *)key, ksize, (void *)value, vsize, (hashfn(key, ksize) & (this->CAPACITY-1)) - idx, OCCUPIED };
  
  if ( LF_RESIZE_TRIGGER_VALUE < this->LOAD_FACTOR )
    return Hresize(this);
  
  return H_SUCESS;
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
  if ( NULL == this || NULL == this->TABLE || NULL == key )
    return;

  const size_t idx = Hfind_slot(this, key, size);
  if ( SIZE_MAX != idx && Hcompare_key_entry( key, size, this->TABLE[idx] ) )
  {
    this->TABLE[idx].state = TOMBSTONE;
    this->TABLE[idx].ksize = 0;
  }

  return;
}
