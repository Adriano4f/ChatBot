#include "LI_Unit.h"

#include "check.h"
#include <stdio.h>
#include <string.h>

static size_t
count_tokens
  (char **Tokens)
{
  size_t i = 0;
  while ( NULL != Tokens[i] )
    ++i;
  return i;
}


static void
test_tokenise_splits_on_delimiters
  (void)
{
  strcpy(GlobalInputBuffer, "hello world, this is a test!\n");

  char **Tokens = Tokenise();
  CHECK(NULL != Tokens);
  CHECK(6 == count_tokens(Tokens));
  CHECK(0 == strcmp(Tokens[0], "hello"));
  CHECK(0 == strcmp(Tokens[5], "test"));

  LI_info info = { Tokens, NULL, SUCESS };
  FreeLI_info(&info);
}


static void
test_tokenise_grows_past_the_initial_capacity
  (void)
{
  /*
    The token array starts at 500 slots, so 510 tokens exercise the resize path.
    Every token is a single character to stay inside INPUT_BUFFER_SIZE.
  */
  size_t written = 0;
  for ( size_t i = 0; i < 510; ++i )
    written += (size_t)snprintf(GlobalInputBuffer + written,
                               sizeof(GlobalInputBuffer) - written, "a ");

  char **Tokens = Tokenise();
  CHECK(NULL != Tokens);
  CHECK(510 == count_tokens(Tokens));

  LI_info info = { Tokens, NULL, SUCESS };
  FreeLI_info(&info);
}


static void
test_tokenise_rejects_input_without_tokens
  (void)
{
  strcpy(GlobalInputBuffer, "   \t\n");
  CHECK(NULL == Tokenise());
}


int
main
  (void)
{
  test_tokenise_splits_on_delimiters();
  test_tokenise_grows_past_the_initial_capacity();
  test_tokenise_rejects_input_without_tokens();

  puts("test_tokenise: all assertions passed");
  return 0;
}
