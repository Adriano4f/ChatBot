#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include <stdint.h>
#include <stddef.h>

void 
PrtError
  (const char *msg,
  int64_t error_code);

void 
PrtDbgError
  (const char *msg,
  const char* error_msg);
  
// == MEMORY MANEGEMENT ==

void 
*PtrVerify
  (void *ptr,
  const char* msg,
  const char* error_msg);
  
#endif
