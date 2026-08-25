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
PrtColored
  (const char *color,
  const char *text)
{
  printf("%s%s%s", Txt(color), text, Txt(CRESET));
}

void 
PrtError
  (const char *msg,
  int64_t error_code)
{
  printf("%s%s ERROR CODE: %s%ld%s\n",
          Txt(RED), msg, Txt(MAGENTA), error_code, Txt(CRESET) );
}

void 
PrtDbgError
  (const char *msg,
  const char* error_msg)
{
  printf("%s%s %s%s%s",
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
