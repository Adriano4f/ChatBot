#include "Tokenizer.h"

#include "Core.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define TOKENS_INITIAL_CAPACITY 64


static int
TokenListPush
  (TokenList *list,
  size_t *capacity,
  const char *start,
  size_t length)
{
  if ( list->count == *capacity )
  {
    size_t grown = *capacity * 2;
    char **items = (char **)DanaRealloc(list->items, grown * sizeof(char *),
                                       "TokenListPush");
    if ( NULL == items )
      return DANA_ENOMEM;

    list->items = items;
    *capacity = grown;
  }

  char *token = (char *)DanaAlloc(length + 1, "TokenListPush");
  if ( NULL == token )
    return DANA_ENOMEM;

  for ( size_t i = 0; i < length; ++i )
    token[i] = (char)tolower((unsigned char)start[i]);
  token[length] = '\0';

  list->items[list->count] = token;
  ++list->count;

  return DANA_OK;
}


int
Tokenize
  (const char *text,
  TokenList *out)
{
  if ( NULL == text || NULL == out )
    return DANA_EINVAL;

  size_t capacity = TOKENS_INITIAL_CAPACITY;
  out->items = (char **)DanaAlloc(capacity * sizeof(char *), "Tokenize");
  out->count = 0;
  if ( NULL == out->items )
    return DANA_ENOMEM;

  const char *cursor = text;
  while ( '\0' != *cursor )
  {
    cursor += strspn(cursor, TOKEN_DELIMITERS);
    size_t length = strcspn(cursor, TOKEN_DELIMITERS);
    if ( 0 == length )
      break;

    int status = TokenListPush(out, &capacity, cursor, length);
    if ( DANA_OK != status )
    {
      TokenListFree(out);
      return status;
    }

    cursor += length;
  }

  return DANA_OK;
}


void
TokenListFree
  (TokenList *list)
{
  if ( NULL == list )
    return;

  for ( size_t i = 0; i < list->count; ++i )
    free(list->items[i]);

  free(list->items);
  list->items = NULL;
  list->count = 0;
}
