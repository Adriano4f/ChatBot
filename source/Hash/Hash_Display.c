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
      Hdisplay_char(this);
      break;

    case INT:
      Hdisplay_int(this);
      break;

    case INT64:
      Hdisplay_int64(this);
      break;
  }

  return;
}

int
Hdisplay_char
  (Hash *this)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const char *key   = this->TABLE[i].key;
    const size_t ksize  = this->TABLE[i].ksize;
    const char *value   = this->TABLE[i].value;
    const size_t vsize  = this->TABLE[i].vsize;
    if (fwrite(key, sizeof(char), ksize, stdout) != ksize)
      longjmp(err, 1);
    printf(" - ");
    if (fwrite(value, sizeof(char), vsize, stdout) != vsize)
      longjmp(err, 1); // In case of error writing
    puts(""); 
  }

  return 0;
}

int
Hdisplay_int
  (Hash *this)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const int *key   = this->TABLE[i].key;
    const int *value   = this->TABLE[i].value;
    if (fwrite(key, sizeof(int), 1, stdout) != 1)
      longjmp(err, 1);
    printf(" - ");
    if (fwrite(value, sizeof(int), 1, stdout) != 1)
      longjmp(err, 1); // In case of error writing
    puts("");
  }
  
  return 0;
}


int
Hdisplay_int64
  (Hash *this)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < this->CAPACITY; ++i )
  {
    if ( OCCUPIED != this->TABLE[i].state)
      continue;
    const int64_t *key   = this->TABLE[i].key;
    const int64_t *value   = this->TABLE[i].value;
    if (fwrite(key, sizeof(int64_t), 1, stdout) != 1)
      longjmp(err, 1);
    printf(" - ");
    if (fwrite(value, sizeof(int64_t), 1, stdout) != 1)
      longjmp(err, 1); // In case of error writing
    puts("");
  }
  
  return 0;
}
