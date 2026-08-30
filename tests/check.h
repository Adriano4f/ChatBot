#ifndef _DANA_TEST_CHECK_
#define _DANA_TEST_CHECK_

/*
  assert() is compiled out in release builds, which is exactly when the tests
  are most likely to run, so the checks are always live.
*/

#include <stdio.h>
#include <stdlib.h>

#define CHECK(cond)                                                          \
  do                                                                         \
  {                                                                          \
    if ( !(cond) )                                                           \
    {                                                                        \
      fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #cond);\
      exit(1);                                                               \
    }                                                                        \
  } while ( 0 )

#endif
