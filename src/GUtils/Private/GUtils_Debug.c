#include "GUtils_internal.h"

// STD
#include <stdio.h>
#include <stdlib.h>

const char*
Txt
  (const char* text)
{
  return text;
}

void 
PrtError
  (const char *msg,
  int64_t error_code)
{
  fprintf(stderr, "%s%s ERROR CODE: %s%ld%s\n",
          Txt(RED), msg, Txt(MAGENTA), error_code, Txt(CRESET) );
}

void 
PrtDbgError
  (const char *msg,
  const char* error_msg)
{
  fprintf(stderr, "%s%s %s%s%s\n",
      Txt(RED), msg, Txt(YELLOW), error_msg, Txt(CRESET) );
}

// == MEMORY MANAGEMENT ==

void 
*PtrVerify
  (void *ptr,
  const char* msg,
  const char* error_msg)
{
  if ( ptr == NULL )
  {
    PrtDbgError(msg, error_msg);
    return NULL;
  }
  else
    return ptr;
}

