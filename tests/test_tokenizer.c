#include "Tokenizer.h"

#include "check.h"

#include <stdio.h>
#include <string.h>


static void
TestSplitsAndLowercases
  (void)
{
  TokenList tokens;
  CHECK(0 == Tokenize("Hello, world! How ARE you?", &tokens));
  CHECK(5 == tokens.count);
  CHECK(0 == strcmp(tokens.items[0], "hello"));
  CHECK(0 == strcmp(tokens.items[1], "world"));
  CHECK(0 == strcmp(tokens.items[2], "how"));
  CHECK(0 == strcmp(tokens.items[3], "are"));
  CHECK(0 == strcmp(tokens.items[4], "you"));
  TokenListFree(&tokens);
}


// strtok used to write into the input; the tokenizer must not
static void
TestLeavesInputAlone
  (void)
{
  char text[] = "one two three";
  TokenList tokens;

  CHECK(0 == Tokenize(text, &tokens));
  CHECK(3 == tokens.count);
  CHECK(0 == strcmp(text, "one two three"));
  TokenListFree(&tokens);

  CHECK(0 == Tokenize(text, &tokens));
  CHECK(3 == tokens.count);
  TokenListFree(&tokens);
}


static void
TestEmptyInput
  (void)
{
  TokenList tokens;

  CHECK(0 == Tokenize("", &tokens));
  CHECK(0 == tokens.count);
  TokenListFree(&tokens);

  CHECK(0 == Tokenize("   \t\n ... ", &tokens));
  CHECK(0 == tokens.count);
  TokenListFree(&tokens);
}


// The token array has to grow past its initial capacity
static void
TestGrowsPastCapacity
  (void)
{
  char text[8000];
  size_t written = 0;
  for ( int i = 0; i < 1000; ++i )
    written += (size_t)snprintf(text + written, sizeof(text) - written,
                                "w%d ", i);

  TokenList tokens;
  CHECK(0 == Tokenize(text, &tokens));
  CHECK(1000 == tokens.count);
  CHECK(0 == strcmp(tokens.items[0], "w0"));
  CHECK(0 == strcmp(tokens.items[999], "w999"));
  TokenListFree(&tokens);
}


static void
TestInvalidArguments
  (void)
{
  TokenList tokens;

  CHECK(0 != Tokenize(NULL, &tokens));
  CHECK(0 != Tokenize("word", NULL));

  TokenListFree(NULL);
}


int
main
  (void)
{
  TestSplitsAndLowercases();
  TestLeavesInputAlone();
  TestEmptyInput();
  TestGrowsPastCapacity();
  TestInvalidArguments();

  return 0;
}
