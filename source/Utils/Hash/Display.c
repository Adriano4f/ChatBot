#include "Utils/Hash/Internal.h"

#include <stdio.h>
#include <setjmp.h> // Satanic things

void
Hdisplay // Debug purposes
  (Hash *self, 
  DisplayType type)
{
  printf("Capacity: %zu\nSize: %zu\n", self->CAPACITY, self->SIZE);

  switch ( type )
  {
    case CHAR:
      Hdisplay_char(self);
      break;

    case INT:
      Hdisplay_int(self);
      break;

    case INT64:
      Hdisplay_int64(self);
      break;
  }

  return;
}

int
Hdisplay_char
  (Hash *self)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < self->CAPACITY; ++i )
  {
    if ( OCCUPIED != self->TABLE[i].state)
      continue;
    const char *key   = self->TABLE[i].key;
    const size_t ksize  = self->TABLE[i].ksize;
    const char *value   = self->TABLE[i].value;
    const size_t vsize  = self->TABLE[i].vsize;
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
  (Hash *self)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < self->CAPACITY; ++i )
  {
    if ( OCCUPIED != self->TABLE[i].state)
      continue;
    const int *key   = self->TABLE[i].key;
    const int *value   = self->TABLE[i].value;
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
  (Hash *self)
{
  jmp_buf err;

  if ( setjmp(err) )
    return -2;

  for ( size_t i = 0; i < self->CAPACITY; ++i )
  {
    if ( OCCUPIED != self->TABLE[i].state)
      continue;
    const int64_t *key   = self->TABLE[i].key;
    const int64_t *value   = self->TABLE[i].value;
    if (fwrite(key, sizeof(int64_t), 1, stdout) != 1)
      longjmp(err, 1);
    printf(" - ");
    if (fwrite(value, sizeof(int64_t), 1, stdout) != 1)
      longjmp(err, 1); // In case of error writing
    puts("");
  }
  
  return 0;
}
