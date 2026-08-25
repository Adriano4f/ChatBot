#ifndef _TEST_FRAMEWORK_
#define _TEST_FRAMEWORK_

/*
  Minimal dependency free unit test framework.

  A test is a void(void) function declared with TEST, registered in the suite
  table of TestRunner.c and executed by RUN_TEST.
*/

#include <stdio.h>
#include <string.h>

extern int TestsRun;
extern int TestsFailed;
extern int CurrentTestFailed;
extern const char *CurrentTestName;

#define TEST(name) void name(void)

#define CHECK(cond, ...)                                    \
  do {                                                      \
    if ( !(cond) )                                          \
    {                                                       \
      CurrentTestFailed = 1;                                \
      printf("    FAIL %s:%d: ", __FILE__, __LINE__);       \
      printf(__VA_ARGS__);                                  \
      printf("\n");                                         \
    }                                                       \
  } while ( 0 )

#define CHECK_TRUE(cond) CHECK((cond), "expected true: %s", #cond)

#define CHECK_FALSE(cond) CHECK(!(cond), "expected false: %s", #cond)

#define CHECK_NULL(ptr) CHECK((ptr) == NULL, "expected NULL: %s", #ptr)

#define CHECK_NOT_NULL(ptr) CHECK((ptr) != NULL, "expected non NULL: %s", #ptr)

#define CHECK_EQ_INT(actual, expected)                      \
  CHECK((long long)(actual) == (long long)(expected),       \
    "%s: expected %lld, got %lld", #actual,                 \
    (long long)(expected), (long long)(actual))

#define CHECK_EQ_SIZE(actual, expected)                     \
  CHECK((size_t)(actual) == (size_t)(expected),             \
    "%s: expected %zu, got %zu", #actual,                   \
    (size_t)(expected), (size_t)(actual))

#define CHECK_EQ_STR(actual, expected)                      \
  CHECK(strcmp((actual), (expected)) == 0,                  \
    "%s: expected \"%s\", got \"%s\"", #actual,             \
    (expected), (actual))

#define CHECK_EQ_MEM(actual, expected, size)                \
  CHECK(memcmp((actual), (expected), (size)) == 0,          \
    "%s: %zu bytes differ from %s", #actual,                \
    (size_t)(size), #expected)

void RunTest(const char *name, void (*fn)(void));

#define RUN_TEST(fn) RunTest(#fn, fn)

/*
  stdout capture, needed by the modules that only report through printf.
  CaptureStdoutStart redirects stdout to a temporary file, CaptureStdoutStop
  restores it and copies what was written into buffer.
*/

int CaptureStdoutStart(void);
size_t CaptureStdoutStop(char *buffer, size_t size);

/*
  Makes writes to stdout fail, to reach the error paths of the functions that
  report through it. FailingStdoutStop puts the real stdout back.
*/
int FailingStdoutStart(void);
void FailingStdoutStop(void);

/* Feeds text to a fresh stdin, so the input reading functions can be tested. */
int RedirectStdin(const char *content, size_t size);
int RedirectStdinEmpty(void);

/* Points stdin at a directory, where every read fails without reaching EOF. */
int RedirectStdinUnreadable(void);

#endif
