#include "TestFramework.h"

#include "IntentP.h"
#include "LI_Unit.h"
#include "LP_Unit.h"

// STD
#include <string.h>

/*
  LP_Unit and IntentP are still stubs, these tests pin the contract their
  callers rely on today: no crash and a neutral answer for any input.
*/

/* == ProcessInput == */

TEST(process_input_succeeds_for_a_tokenised_input)
{
  char *tokens[] = { "hola", "mundo", NULL };
  char input[] = "hola mundo";
  const LI_info info = { tokens, input };

  CHECK_EQ_INT( ProcessInput(info), 0 );
}

TEST(process_input_succeeds_for_an_empty_input)
{
  const LI_info info = { NULL, NULL };

  CHECK_EQ_INT( ProcessInput(info), 0 );
}

/* == GetIntent == */

TEST(get_intent_defaults_to_statement)
{
  char *tokens[] = { "hola", NULL };
  char input[] = "hola";
  const LI_info info = { tokens, input };

  const Intent_info intent = GetIntent(info);

  CHECK_EQ_INT( intent.intent, STATEMENT );
}

TEST(get_intent_handles_an_empty_input)
{
  const LI_info info = { NULL, NULL };

  const Intent_info intent = GetIntent(info);

  CHECK_EQ_INT( intent.intent, STATEMENT );
}

TEST(get_intent_does_not_consume_the_input)
{
  char *tokens[] = { "hola", NULL };
  char input[] = "hola";
  const LI_info info = { tokens, input };

  GetIntent(info);

  CHECK_EQ_STR( input, "hola" );
  CHECK_EQ_STR( tokens[0], "hola" );
}

void
RegisterLPUnitTests
  (void)
{
  printf("LP_Unit / IntentP\n");

  RUN_TEST(process_input_succeeds_for_a_tokenised_input);
  RUN_TEST(process_input_succeeds_for_an_empty_input);

  RUN_TEST(get_intent_defaults_to_statement);
  RUN_TEST(get_intent_handles_an_empty_input);
  RUN_TEST(get_intent_does_not_consume_the_input);

  return;
}
