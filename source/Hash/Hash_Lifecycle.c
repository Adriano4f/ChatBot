#include "Hash_internal.h"
  #include "GUtils.h"

// STD
#include <stdlib.h>

int 
Hresize
  (Hash *this)
{   
  size_t OLD_CAPACITY = this->CAPACITY;
  size_t NEW_CAPACITY = this->CAPACITY*2;
  
  void *tmp = calloc(NEW_CAPACITY, sizeof(RHEntry));;
  if ( tmp == NULL )
    return 1;

  RHEntry *TABLE = this->TABLE;
  this->TABLE = (RHEntry *)tmp;
  this->SIZE = 0;
  this->CAPACITY = NEW_CAPACITY;

  const int err_Hrehash = Hrehash(this, TABLE);
  if ( err_Hrehash )
  {
    this->TABLE = TABLE;
    this->CAPACITY = OLD_CAPACITY;
    return err_Hrehash;
  }

  this->LOAD_FACTOR /= 2;
  free(TABLE); // No memory leak, TODO: For deep copy dealloc.
  
  return 0;
}


void
Hdestroy
  (Hash *this)
{
  free(this->TABLE);
  free(this);
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
  ret->LOAD_FACTOR = HASH_LOAD_FACTOR(SIZE, CAPACITY);

  ret->TABLE = (RHEntry *)calloc( CAPACITY, sizeof(RHEntry) ); // TODO: create a calloc wrapper for error handling

  ret->insert = Hinsert;
  ret->delete = Hdelete;
  ret->fetch = Hfetch;
  ret->display = Hdisplay;
  ret->resize = Hresize;
  ret->destroy = Hdestroy;

  return ret;
}
