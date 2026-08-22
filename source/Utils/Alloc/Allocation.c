#include "Utils/Alloc/Alloc.h"
  #include "Utils/Debug/Debug.h"
// STD
#include <stdlib.h>


void *
AllocPtr (size_t nmemb)
{
  void *tmp = malloc(nmemb);
  return PtrVerify
  (   
    tmp, 
    "Allocation Error.", 
    "LI -> AllocCharPtr" 
  );
}


void *
ReallocPtr (size_t nmemb, void *ptr)
{
  void *tmp = realloc(ptr, nmemb);
  return PtrVerify
  (   
    tmp, 
    "Reallocation Error.", 
    "LI -> ReallocCharPtr" 
  );
}


void **
AllocPPtr (size_t nmemb)
{
  void **tmp = malloc(nmemb);
  return PtrVerify
  (   
    (void *)tmp, 
    "Allocation Error.", 
    "LI -> AllocCharPPtr" 
  );
}


void **
ReallocPPtr (size_t nmemb, void **ptr)
{
  void **tmp = realloc(ptr, nmemb);
  return PtrVerify
  (   
    (void *)tmp, 
    "Reallocation Error.", 
    "LI -> ReallocCharPPtr" 
  );
}
