#include "Hash_internal.h"

#include <stdio.h>

int
Hdisplay // Debug purposes
  (Hash *this, 
  DisplayType type)
{
  if ( NULL == this || NULL == this->TABLE )
    return H_INVALID_ARG;

  printf("Capacity: %zu\nSize: %zu\n", this->CAPACITY, this->SIZE);

  switch ( type )
  {
    case CHAR:
      return Hdisplay_char(this);

    case INT:
      return Hdisplay_int(this);

    case INT64:
      return Hdisplay_int64(this);
  }

  return H_INVALID_ARG;
}

int
Hdisplay_char
  (Hash *this)
{
  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const char *key   = this->TABLE[i].key;
    const size_t ksize  = this->TABLE[i].ksize;
    const char *value   = this->TABLE[i].value;
    const size_t vsize  = this->TABLE[i].vsize;
    if (fwrite(key, sizeof(char), ksize, stdout) != ksize)
      return H_WRITE_FAILED;
    printf(" - ");
    if (fwrite(value, sizeof(char), vsize, stdout) != vsize)
      return H_WRITE_FAILED;
    puts(""); 
  }

  return H_SUCESS;
}

int
Hdisplay_int
  (Hash *this)
{
  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const int *key   = this->TABLE[i].key;
    const int *value   = this->TABLE[i].value;
    if (fwrite(key, sizeof(int), 1, stdout) != 1)
      return H_WRITE_FAILED;
    printf(" - ");
    if (fwrite(value, sizeof(int), 1, stdout) != 1)
      return H_WRITE_FAILED;
    puts("");
  }
  
  return H_SUCESS;
}


int
Hdisplay_int64
  (Hash *this)
{
  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const int64_t *key   = this->TABLE[i].key;
    const int64_t *value   = this->TABLE[i].value;
    if (fwrite(key, sizeof(int64_t), 1, stdout) != 1)
      return H_WRITE_FAILED;
    printf(" - ");
    if (fwrite(value, sizeof(int64_t), 1, stdout) != 1)
      return H_WRITE_FAILED;
    puts("");
  }
  
  return H_SUCESS;
}
