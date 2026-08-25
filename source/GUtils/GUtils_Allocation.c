#include "GUtils_internal.h"

// STD
#include <stdlib.h>


void
*AllocPtr
  (size_t nmemb)
{
  return PtrVerify(malloc(nmemb), "Allocation Error.", "GUtils -> AllocPtr");
}


void 
*ReallocPtr
  (size_t nmemb, void *ptr)
{
  return PtrVerify(realloc(ptr, nmemb), "Reallocation Error.", "GUtils -> ReallocPtr");
}


void 
**AllocPPtr
  (size_t nmemb)
{
  return (void **)AllocPtr(nmemb);
}


void 
**ReallocPPtr
  (size_t nmemb, void **ptr)
{
  return (void **)ReallocPtr(nmemb, ptr);
}
