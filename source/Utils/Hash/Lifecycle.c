#include "Utils/Hash/Internal.h"
#include "Utils/Alloc/Alloc.h"
// STD
#include <stdlib.h>

int 
Hresize
  (Hash *self)
{   
  size_t OLD_CAPACITY = self->CAPACITY;
  size_t NEW_CAPACITY = self->CAPACITY*2;
  
  void *tmp = calloc(NEW_CAPACITY, sizeof(RHEntry));;
  if ( tmp == NULL )
    return 1;

  RHEntry *TABLE = self->TABLE;
  self->TABLE = (RHEntry *)tmp;
  self->SIZE = 0;
  self->CAPACITY = NEW_CAPACITY;

  const int err_Hrehash = Hrehash(self, TABLE);
  if ( err_Hrehash )
  {
    self->TABLE = TABLE;
    self->CAPACITY = OLD_CAPACITY;
    return err_Hrehash;
  }

  self->LOAD_FACTOR /= 2;
  free(TABLE); // No memory leak, TODO: For deep copy dealloc.
  
  return 0;
}


void
Hdestroy
  (Hash *self)
{
  free(self->TABLE);
  free(self);
}


Hash
*Hinit
  (void)
{
  Hash *ret = (Hash *)AllocPtr( sizeof(Hash) );

  size_t CAPACITY = 8;
  size_t SIZE = 0;

  ret->CAPACITY = CAPACITY;
  ret->SIZE = SIZE;
  ret->LOAD_FACTOR = SIZE*1000/CAPACITY;

  ret->TABLE = (RHEntry *)calloc( CAPACITY, sizeof(RHEntry) ); // TODO: create a calloc wrapper for error handling

  ret->insert = Hinsert;
  ret->delete = Hdelete;
  ret->fetch = Hfetch;
  ret->resize = Hresize;
  ret->destroy = Hdestroy;

  #ifdef DEBUG
  ret->display = Hdisplay;
  #endif
  
  return ret;
}
