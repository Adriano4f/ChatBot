#include "GUtils_internal.h"

// STD
#include <stdlib.h>


void
*AllocPtr
  (size_t nmemb)
{
  void *tmp = malloc(nmemb);
  return PtrVerify
  (   
    tmp, 
    "Allocation Error.", 
    "GUtils -> AllocPtr" 
  );
}


void
*CallocPtr
  (size_t nmemb, size_t size)
{
  void *tmp = calloc(nmemb, size);
  return PtrVerify
  (
    tmp,
    "Allocation Error.",
    "GUtils -> CallocPtr"
  );
}


void 
*ReallocPtr
  (size_t nmemb, void *ptr)
{
  void *tmp = realloc(ptr, nmemb);
  return PtrVerify
  (   
    tmp, 
    "Reallocation Error.", 
    "GUtils -> ReallocPtr" 
  );
}


void 
**AllocPPtr
  (size_t nmemb)
{
  void **tmp = malloc(nmemb);
  return PtrVerify
  (   
    (void *)tmp, 
    "Allocation Error.", 
    "GUtils -> AllocPPtr" 
  );
}


void 
**ReallocPPtr
  (size_t nmemb, void **ptr)
{
  void **tmp = realloc(ptr, nmemb);
  return PtrVerify
  (   
    (void *)tmp, 
    "Reallocation Error.", 
    "GUtils -> ReallocPPtr" 
  );
}
