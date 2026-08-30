#include "Corpus.h"

#include "Core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
CorpusRead
  (const char *path,
  char **out_text,
  size_t *out_size)
{
  if ( NULL == path || NULL == out_text || NULL == out_size )
    return DANA_EINVAL;

  *out_text = NULL;
  *out_size = 0;

  FILE *file = fopen(path, "rb");
  if ( NULL == file )
    return DANA_EIO;

  if ( 0 != fseek(file, 0, SEEK_END) )
  {
    fclose(file);
    return DANA_EIO;
  }

  long end = ftell(file);
  if ( end < 0 || 0 != fseek(file, 0, SEEK_SET) )
  {
    fclose(file);
    return DANA_EIO;
  }

  size_t size = (size_t)end;
  char *text = (char *)DanaAlloc(size + 1, "CorpusRead");
  if ( NULL == text )
  {
    fclose(file);
    return DANA_ENOMEM;
  }

  size_t read = fread(text, 1, size, file);
  int failed = 0 != ferror(file);
  fclose(file);

  if ( failed )
  {
    free(text);
    return DANA_EIO;
  }

  text[read] = '\0';
  *out_text = text;
  *out_size = read;

  return DANA_OK;
}


int
CorpusReadLine
  (char *buffer,
  size_t size)
{
  if ( NULL == buffer || 0 == size )
    return DANA_EINVAL;

  if ( NULL == fgets(buffer, (int)size, stdin) )
    return feof(stdin) ? DANA_EEOF : DANA_EIO;

  buffer[strcspn(buffer, "\n")] = '\0';

  return DANA_OK;
}
