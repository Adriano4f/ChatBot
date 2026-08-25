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

  const size_t OLD_SIZE = this->SIZE;
  RHEntry *TABLE = this->TABLE;
  this->TABLE = (RHEntry *)tmp;
  this->SIZE = 0;
  this->CAPACITY = NEW_CAPACITY;

  const int err_Hrehash = Hrehash(this, TABLE, OLD_CAPACITY);
  if ( err_Hrehash )
  {
    free(this->TABLE);
    this->TABLE = TABLE;
    this->CAPACITY = OLD_CAPACITY;
    this->SIZE = OLD_SIZE;
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
  if ( ret == NULL )
    return NULL;

  size_t CAPACITY = 8;
  size_t SIZE = 0;

  ret->CAPACITY = CAPACITY;
  ret->SIZE = SIZE;
  ret->LOAD_FACTOR = SIZE*1000/CAPACITY;

  ret->TABLE = (RHEntry *)calloc( CAPACITY, sizeof(RHEntry) ); // TODO: create a calloc wrapper for error handling
  if ( ret->TABLE == NULL )
  {
    free(ret);
    return NULL;
  }

  ret->insert = Hinsert;
  ret->delete = Hdelete;
  ret->fetch = Hfetch;
  ret->display = Hdisplay;
  ret->resize = Hresize;
  ret->destroy = Hdestroy;

  return ret;
}
