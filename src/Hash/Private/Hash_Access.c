#include "Hash_internal.h"

const RHEntry
*Hfetch
  (Hash *this, 
  const void *key,
  const size_t size)
{
  if ( NULL == this || NULL == this->TABLE || NULL == key )
    return NULL;

  const size_t idx = Hfind_slot(this, key, size);
  if ( SIZE_MAX == idx || !Hcompare_key_entry( key, size, this->TABLE[idx] ) )
    return NULL;
  
  const RHEntry *toret = &(this->TABLE[idx]);

  return toret;
}
