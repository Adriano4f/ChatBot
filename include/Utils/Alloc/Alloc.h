#ifndef UTILS_ALLOC_H
#define UTILS_ALLOC_H

#include <stdint.h>
#include <stddef.h>

void 
*AllocPtr
  (size_t nmemb);

void
*ReallocPtr
  (size_t nmemb, void *ptr);

void 
**AllocPPtr
  (size_t nmemb);

void 
**ReallocPPtr
  (size_t nmemb, void **ptr);

#endif