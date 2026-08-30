#include "Hash_internal.h"

size_t
hashfn
  (const void *key,
  const size_t size)
{
  /*
    Per byte FNV-1a hashing function
  */
  size_t hash = 0xcbf29ce484222325; // FNV-1a 64-bit offset basis
  for( size_t i = 0; i < size; ++i )
  {
    hash ^= (size_t) *((const unsigned char *)key + i); // Signed chars would sign extend bytes >= 0x80
    hash *= 0x100000001b3; // FNV-1a prime
  }
  return hash;
}
