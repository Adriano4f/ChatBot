#include "TestFramework.h"

#include "GUtils.h" // Colors of the debug echo
#include "LI_Unit.h"

// STD
#include <stdlib.h>
#include <string.h>

/*
  Every function of the unit works on the shared GlobalInputBuffer, so each test
  sets it explicitly instead of relying on what the previous one left behind.

  Tokenise is intentionally exercised with short inputs only: it sizes its
  pointer array in bytes instead of pointers, so bigger inputs walk out of the
  allocation (see the notes in tests/README.md).
*/

static void
SetGlobalInput
  (const char *text)
{
  memset(GlobalInputBuffer, 0, sizeof(GlobalInputBuffer));
  memcpy(GlobalInputBuffer, text, strlen(text));
  return;
}

/* == GetInput == */

TEST(get_input_reads_a_line_into_the_global_buffer)
{
  char out[2048];
  SetGlobalInput("");
  RedirectStdin("hola mundo\n", strlen("hola mundo\n"));

  CaptureStdoutStart();
  const int err = GetInput();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, 0 );
  CHECK_EQ_STR( GlobalInputBuffer, "hola mundo\n" );
  CHECK_EQ_STR( out, CYAN "hola mundo\n" CRESET );
}

TEST(get_input_stops_at_the_first_newline)
{
  char out[2048];
  SetGlobalInput("");
  RedirectStdin("uno\ndos\n", strlen("uno\ndos\n"));

  CaptureStdoutStart();
  GetInput();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_STR( GlobalInputBuffer, "uno\n" );
}

TEST(get_input_truncates_a_line_longer_than_the_buffer)
{
  char out[4096];
  char line[INPUT_BUFFER_SIZE + 64];
  SetGlobalInput("");

  memset(line, 'a', sizeof(line) - 2);
  line[sizeof(line) - 2] = '\n';
  line[sizeof(line) - 1] = '\0';
  RedirectStdin(line, strlen(line));

  CaptureStdoutStart();
  const int err = GetInput();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, 0 );
  CHECK_EQ_SIZE( strlen(GlobalInputBuffer), INPUT_BUFFER_SIZE - 1 );

  return;
}

TEST(get_input_reports_end_of_file)
{
  char out[512];
  SetGlobalInput("");
  RedirectStdinEmpty();

  CaptureStdoutStart();
  const int err = GetInput();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, EOF_ERROR );
  CHECK_NOT_NULL( strstr(out, "End of file reached.") );
  CHECK_NOT_NULL( strstr(out, "254000") );
}

TEST(get_input_reports_a_read_failure)
{
  char out[512];
  SetGlobalInput("");
  RedirectStdinUnreadable();

  CaptureStdoutStart();
  const int err = GetInput();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_EQ_INT( err, INPUT_READ_FAILED_LI );
  CHECK_NOT_NULL( strstr(out, "Error reading input.") );
  CHECK_NOT_NULL( strstr(out, "255000") );
}

/* == Tokenise == */

TEST(tokenise_returns_null_for_an_empty_buffer)
{
  SetGlobalInput("");

  CHECK_NULL( Tokenise() );
}

TEST(tokenise_returns_null_when_the_input_is_only_delimiters)
{
  SetGlobalInput(" \t\n");

  CHECK_NULL( Tokenise() );
}

TEST(tokenise_splits_a_single_word)
{
  SetGlobalInput("hola\n");

  char **tokens = Tokenise();

  CHECK_NOT_NULL( tokens );
  if ( tokens == NULL )
    return;

  /*
    The first token is shrunk to strlen bytes, dropping its terminator, so it is
    compared byte by byte instead of as a string.
  */
  CHECK_EQ_MEM( tokens[0], "hola", strlen("hola") );
  CHECK_NULL( tokens[1] );
}

TEST(tokenise_splits_on_punctuation_and_spaces)
{
  char out[512];
  SetGlobalInput("hola, mundo!\n");

  CaptureStdoutStart();
  char **tokens = Tokenise();
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NOT_NULL( tokens );
  if ( tokens == NULL )
    return;

  CHECK_EQ_MEM( tokens[0], "hola", strlen("hola") );
  CHECK_EQ_STR( tokens[1], "mundo" );
  CHECK_NULL( tokens[2] );
  CHECK_NOT_NULL( strstr(out, "mundo") ); // Debug echo of every token but the first
}

TEST(tokenise_consumes_the_global_buffer)
{
  char out[512];
  SetGlobalInput("hola mundo\n");

  CaptureStdoutStart();
  Tokenise();
  CaptureStdoutStop(out, sizeof(out));

  /* strtok writes terminators over the delimiters it finds. */
  CHECK_EQ_STR( GlobalInputBuffer, "hola" );
}

/* == HandleInput / Linked == */

TEST(handle_input_delegates_to_linked_when_linked)
{
  const LI_info info = HandleInput(1, NULL);

  CHECK_NULL( info.Tokens );
  CHECK_NULL( info.Input );
}

TEST(linked_is_not_implemented_yet)
{
  const LI_info info = Linked(NULL);

  CHECK_NULL( info.Tokens );
  CHECK_NULL( info.Input );
}

TEST(handle_input_tokenises_the_line_it_read)
{
  char out[2048];
  SetGlobalInput("");
  RedirectStdin("hola mundo\n", strlen("hola mundo\n"));

  CaptureStdoutStart();
  const LI_info info = HandleInput(0, NULL);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NOT_NULL( info.Tokens );
  CHECK_NOT_NULL( info.Input );
  if ( info.Tokens == NULL || info.Input == NULL )
    return;

  CHECK_EQ_MEM( info.Tokens[0], "hola", strlen("hola") );
  CHECK_EQ_STR( info.Tokens[1], "mundo" );
  CHECK_NULL( info.Tokens[2] );

  /*
    Input is copied after Tokenise already cut the buffer at the first
    delimiter, so it only holds the first token.
  */
  CHECK_EQ_STR( info.Input, "hola" );
  CHECK_TRUE( info.Input != GlobalInputBuffer ); // Owned copy

  free(info.Input);
}

TEST(handle_input_returns_no_tokens_for_an_empty_line)
{
  char out[512];
  SetGlobalInput("");
  RedirectStdin("\n", 1);

  CaptureStdoutStart();
  const LI_info info = HandleInput(0, NULL);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NULL( info.Tokens );
  CHECK_TRUE( info.Input == GlobalInputBuffer ); // Borrowed, not a copy
  CHECK_EQ_STR( GlobalInputBuffer, "\n" );
}

TEST(handle_input_returns_no_tokens_on_end_of_file)
{
  char out[512];
  SetGlobalInput("");
  RedirectStdinEmpty();

  CaptureStdoutStart();
  const LI_info info = HandleInput(0, NULL);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NULL( info.Tokens );
  CHECK_NOT_NULL( strstr(out, "End of file reached.") );
}

TEST(handle_input_returns_no_tokens_on_a_read_failure)
{
  char out[512];
  SetGlobalInput("");
  RedirectStdinUnreadable();

  CaptureStdoutStart();
  const LI_info info = HandleInput(0, NULL);
  CaptureStdoutStop(out, sizeof(out));

  CHECK_NULL( info.Tokens );
  CHECK_NOT_NULL( strstr(out, "Error reading input.") );
}

/* == Globals == */

TEST(letters_graph_starts_zeroed)
{
  int sum = 0;

  for ( size_t i = 0; i < 255; ++i )
    for ( size_t j = 0; j < 255; ++j )
      sum += LettersGraph[i][j];

  CHECK_EQ_INT( sum, 0 );
}

void
RegisterLIUnitTests
  (void)
{
  printf("LI_Unit\n");

  RUN_TEST(get_input_reads_a_line_into_the_global_buffer);
  RUN_TEST(get_input_stops_at_the_first_newline);
  RUN_TEST(get_input_truncates_a_line_longer_than_the_buffer);
  RUN_TEST(get_input_reports_end_of_file);
  RUN_TEST(get_input_reports_a_read_failure);

  RUN_TEST(tokenise_returns_null_for_an_empty_buffer);
  RUN_TEST(tokenise_returns_null_when_the_input_is_only_delimiters);
  RUN_TEST(tokenise_splits_a_single_word);
  RUN_TEST(tokenise_splits_on_punctuation_and_spaces);
  RUN_TEST(tokenise_consumes_the_global_buffer);

  RUN_TEST(handle_input_delegates_to_linked_when_linked);
  RUN_TEST(linked_is_not_implemented_yet);
  RUN_TEST(handle_input_tokenises_the_line_it_read);
  RUN_TEST(handle_input_returns_no_tokens_for_an_empty_line);
  RUN_TEST(handle_input_returns_no_tokens_on_end_of_file);
  RUN_TEST(handle_input_returns_no_tokens_on_a_read_failure);

  RUN_TEST(letters_graph_starts_zeroed);

  return;
}
