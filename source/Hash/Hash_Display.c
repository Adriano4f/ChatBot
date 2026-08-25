#include "Hash_internal.h"

#include <stdio.h>
#include <setjmp.h> // Satanic things

void
Hdisplay // Debug purposes
  (Hash *this, 
  DisplayType type)
{
  printf("Capacity: %zu\nSize: %zu\n", this->CAPACITY, this->SIZE);

  switch ( type )
  {
    case CHAR:
      Hdisplay_entries(this, sizeof(char), true);
      break;

    case INT:
      Hdisplay_entries(this, sizeof(int), false);
      break;

    case INT64:
      Hdisplay_entries(this, sizeof(int64_t), false);
      break;
  }

  return;
}

int
Hdisplay_entries
  (Hash *this,
  size_t elemsize,
  bool use_entry_sizes)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const void *key   = this->TABLE[i].key;
    const size_t ksize = use_entry_sizes ? this->TABLE[i].ksize : 1;
    const void *value   = this->TABLE[i].value;
    const size_t vsize = use_entry_sizes ? this->TABLE[i].vsize : 1;
    if (fwrite(key, elemsize, ksize, stdout) != ksize)
      longjmp(err, 1);
    printf(" - ");
    if (fwrite(value, elemsize, vsize, stdout) != vsize)
      longjmp(err, 1); // In case of error writing
    puts("");
  }
  
  return 0;
}
