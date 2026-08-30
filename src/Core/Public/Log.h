#ifndef DANA_LOG_H
#define DANA_LOG_H

// Terminal colours
#define C_RESET   "\033[0m"
#define C_RED     "\033[0;31m"
#define C_GREEN   "\033[0;32m"
#define C_YELLOW  "\033[0;33m"
#define C_BLUE    "\033[0;34m"
#define C_MAGENTA "\033[0;35m"
#define C_CYAN    "\033[0;36m"

void
LogError
  (const char *what,
  const char *where);

// Reports `status` (a DANA_* code) with its text
void
LogStatus
  (const char *what,
  const char *where,
  int status);

void
LogInfo
  (const char *fmt,
  ...);

#endif
