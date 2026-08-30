#ifndef DANA_CORPUS_H
#define DANA_CORPUS_H

#include <stddef.h>

/*
  Reads a whole text file into memory. On success *out_text is a NUL-terminated
  buffer the caller frees and *out_size its length in bytes.
*/
int
CorpusRead
  (const char *path,
  char **out_text,
  size_t *out_size);

/*
  Reads one line from stdin into `buffer` (NUL-terminated, newline stripped).
  Returns DANA_EEOF when the input is exhausted.
*/
int
CorpusReadLine
  (char *buffer,
  size_t size);

#endif
