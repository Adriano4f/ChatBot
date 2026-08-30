#include "Core.h"

#include "Log.h"

#include <stdlib.h>


const char *
DanaStatusText
  (int status)
{
  switch ( status )
  {
    case DANA_OK:     return "ok";
    case DANA_EINVAL: return "invalid argument";
    case DANA_ENOMEM: return "out of memory";
    case DANA_EIO:    return "input/output failure";
    case DANA_EEOF:   return "end of input";
    default:          return "unknown status";
  }
}


void *
DanaAlloc
  (size_t size,
  const char *where)
{
  void *ptr = malloc(size);
  if ( NULL == ptr )
    LogError("allocation failed", where);
  return ptr;
}


void *
DanaCalloc
  (size_t nmemb,
  size_t size,
  const char *where)
{
  void *ptr = calloc(nmemb, size);
  if ( NULL == ptr )
    LogError("allocation failed", where);
  return ptr;
}


void *
DanaRealloc
  (void *ptr,
  size_t size,
  const char *where)
{
  void *grown = realloc(ptr, size);
  if ( NULL == grown )
    LogError("reallocation failed", where);
  return grown;
}
