#include "Utils/Hash/Internal.h"

const RHEntry
*Hfetch
  (Hash *self, 
  const void *key,
  const size_t size)
{
  const size_t idx = Hfind_slot(self, key, size);
  if ( SIZE_MAX == idx || !Hcompare_key_entry( key, size, self->TABLE[idx] ) )
    return NULL;
  
  const RHEntry *toret = &(self->TABLE[idx]);

  return toret;
}
