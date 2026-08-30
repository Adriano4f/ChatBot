#ifndef DANA_CORE_H
#define DANA_CORE_H

#include <stddef.h>
#include <stdint.h>

// Status codes returned by every Dana function that can fail
#define DANA_OK        0
#define DANA_EINVAL    1  // Invalid argument
#define DANA_ENOMEM    2  // Allocation failed
#define DANA_EIO       3  // Read or write failed
#define DANA_EEOF      4  // Input is exhausted

const char *
DanaStatusText
  (int status);

// Allocation: same contract as libc, but a failure is reported before returning
void *
DanaAlloc
  (size_t size,
  const char *where);

void *
DanaCalloc
  (size_t nmemb,
  size_t size,
  const char *where);

void *
DanaRealloc
  (void *ptr,
  size_t size,
  const char *where);

#endif
