#define _POSIX_C_SOURCE 200809L

#include "TestFramework.h"

// STD
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int TestsRun = 0;
int TestsFailed = 0;
int CurrentTestFailed = 0;
const char *CurrentTestName = NULL;

void
RunTest
  (const char *name,
  void (*fn)(void))
{
  CurrentTestName = name;
  CurrentTestFailed = 0;
  ++TestsRun;

  fn();

  if ( CurrentTestFailed )
  {
    ++TestsFailed;
    printf("  [FAIL] %s\n", name);
  }
  else
    printf("  [ ok ] %s\n", name);

  fflush(stdout);
  return;
}

/* == STDOUT CAPTURE == */

static FILE *CaptureFile = NULL;
static int SavedStdout = -1;

int
CaptureStdoutStart
  (void)
{
  if ( CaptureFile != NULL )
    return -1;

  CaptureFile = tmpfile();
  if ( CaptureFile == NULL )
    return -1;

  fflush(stdout);
  SavedStdout = dup(STDOUT_FILENO);
  if ( SavedStdout < 0 || dup2(fileno(CaptureFile), STDOUT_FILENO) < 0 )
  {
    fclose(CaptureFile);
    CaptureFile = NULL;
    return -1;
  }

  return 0;
}

size_t
CaptureStdoutStop
  (char *buffer,
  size_t size)
{
  if ( CaptureFile == NULL || size == 0 )
    return 0;

  fflush(stdout);
  dup2(SavedStdout, STDOUT_FILENO);
  close(SavedStdout);
  SavedStdout = -1;

  rewind(CaptureFile);
  const size_t read = fread(buffer, sizeof(char), size - 1, CaptureFile);
  buffer[read] = '\0';

  fclose(CaptureFile);
  CaptureFile = NULL;

  return read;
}

/* == FAILING STDOUT == */

static int SavedStdoutFail = -1;

int
FailingStdoutStart
  (void)
{
  const int full = open("/dev/full", O_WRONLY); // Every write returns ENOSPC
  if ( full < 0 )
    return -1;

  fflush(stdout);
  SavedStdoutFail = dup(STDOUT_FILENO);
  if ( SavedStdoutFail < 0 || dup2(full, STDOUT_FILENO) < 0 )
  {
    close(full);
    return -1;
  }
  close(full);

  setvbuf(stdout, NULL, _IONBF, 0); // The failure shows up on the write, not on the flush

  return 0;
}

void
FailingStdoutStop
  (void)
{
  if ( SavedStdoutFail < 0 )
    return;

  clearerr(stdout);
  dup2(SavedStdoutFail, STDOUT_FILENO);
  close(SavedStdoutFail);
  SavedStdoutFail = -1;

  setvbuf(stdout, NULL, _IOLBF, BUFSIZ);

  return;
}

/* == STDIN REDIRECTION == */

static char StdinPath[] = "/tmp/chatbot_test_stdin_XXXXXX";

int
RedirectStdin
  (const char *content,
  size_t size)
{
  char path[sizeof(StdinPath)];
  memcpy(path, StdinPath, sizeof(StdinPath));

  const int fd = mkstemp(path);
  if ( fd < 0 )
    return -1;

  if ( size != 0 && (size_t)write(fd, content, size) != size )
  {
    close(fd);
    unlink(path);
    return -1;
  }
  close(fd);

  const int failed = ( freopen(path, "r", stdin) == NULL );
  unlink(path); // The file stays alive while stdin holds it open
  clearerr(stdin);

  return failed ? -1 : 0;
}

int
RedirectStdinEmpty
  (void)
{
  return RedirectStdin("", 0);
}

int
RedirectStdinUnreadable
  (void)
{
  if ( freopen("/tmp", "r", stdin) == NULL )
    return -1;

  clearerr(stdin);

  return 0;
}
