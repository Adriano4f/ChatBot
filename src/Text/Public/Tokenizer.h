#ifndef DANA_TOKENIZER_H
#define DANA_TOKENIZER_H

#include <stddef.h>

#define TOKEN_DELIMITERS " \t\r\n.,!?;:\"()"

typedef struct TokenList
{
  char **items;
  size_t count;
} TokenList;

/*
  Splits `text` on TOKEN_DELIMITERS, lowercasing every token, and fills `out`
  with owned copies. `text` is not modified, so the same buffer can be
  tokenized twice. An empty input yields count 0, not an error.
*/
int
Tokenize
  (const char *text,
  TokenList *out);

void
TokenListFree
  (TokenList *list);

#endif
