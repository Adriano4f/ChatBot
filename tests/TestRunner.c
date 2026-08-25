#include "TestFramework.h"

// STD
#include <stdio.h>

void RegisterGUtilsTests(void);
void RegisterHashTests(void);
void RegisterLIUnitTests(void);
void RegisterLPUnitTests(void);

int
main
  (void)
{
  RegisterGUtilsTests();
  RegisterHashTests();
  RegisterLIUnitTests();
  RegisterLPUnitTests();

  printf("\n%d tests, %d failed\n", TestsRun, TestsFailed);

  return TestsFailed == 0 ? 0 : 1;
}
