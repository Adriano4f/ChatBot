#include "Utils/Debug/Debug.h"
#include "Utils/Console/Console.h"

// STD
#include <stdio.h>
#include <stdlib.h>


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

