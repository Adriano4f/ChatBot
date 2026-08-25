#include "Hash_internal.h"
  #include "GUtils.h"

// STD
#include <stdlib.h>

int 
Hresize
  (Hash *this)
{   
  if ( NULL == this || NULL == this->TABLE )
    return H_INVALID_ARG;

  const size_t OLD_CAPACITY = this->CAPACITY;
  const size_t OLD_SIZE = this->SIZE;
  const size_t OLD_LOAD_FACTOR = this->LOAD_FACTOR;
  const size_t NEW_CAPACITY = this->CAPACITY*2;
  
  void *tmp = CallocPtr(NEW_CAPACITY, sizeof(RHEntry));
  if ( tmp == NULL )
    return H_ALLOC_FAILED;

  RHEntry *TABLE = this->TABLE;
  this->TABLE = (RHEntry *)tmp;
  this->SIZE = 0;
  this->CAPACITY = NEW_CAPACITY;
  this->LOAD_FACTOR = 0;

  const int err_Hrehash = Hrehash(this, TABLE, OLD_CAPACITY);
  if ( err_Hrehash )
  {
    PrtError( "Rehash failed, keeping the previous table.", err_Hrehash );
    free(this->TABLE);
    this->TABLE = TABLE;
    this->CAPACITY = OLD_CAPACITY;
    this->SIZE = OLD_SIZE;
    this->LOAD_FACTOR = OLD_LOAD_FACTOR;
    return err_Hrehash;
  }

  free(TABLE); // No memory leak, TODO: For deep copy dealloc.
  
  return H_SUCESS;
}


void
Hdestroy
  (Hash *this)
{
  if ( NULL == this )
    return;

  free(this->TABLE);
  free(this);
}


Hash
*Hinit
  (void)
{
  Hash *ret = (Hash *)AllocPtr( sizeof(Hash) );
  if ( NULL == ret )
    return NULL;

  size_t CAPACITY = 8;
  size_t SIZE = 0;

  ret->CAPACITY = CAPACITY;
  ret->SIZE = SIZE;
  ret->LOAD_FACTOR = SIZE*1000/CAPACITY;

  ret->TABLE = (RHEntry *)CallocPtr( CAPACITY, sizeof(RHEntry) );
  if ( NULL == ret->TABLE )
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
