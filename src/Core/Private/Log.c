#include "Log.h"

#include "Core.h"

#include <stdarg.h>
#include <stdio.h>


void
LogError
  (const char *what,
  const char *where)
{
  fprintf(stderr, "%s%s%s in %s%s%s\n",
          C_RED, what, C_RESET, C_YELLOW, where, C_RESET);
}


void
LogStatus
  (const char *what,
  const char *where,
  int status)
{
  fprintf(stderr, "%s%s%s in %s%s%s: %s\n",
          C_RED, what, C_RESET, C_YELLOW, where, C_RESET, DanaStatusText(status));
}


void
LogInfo
  (const char *fmt,
  ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
}
