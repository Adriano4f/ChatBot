#include "Chat.h"

#include "Core.h"
#include "Corpus.h"
#include "Log.h"
#include "Tokenizer.h"

#include <stdio.h>
#include <string.h>


ChatSettings
ChatDefaults
  (void)
{
  ChatSettings settings;
  settings.temperature = 0.9;
  settings.max_reply = 24;

  return settings;
}


/*
  The word the reply starts from: the last word of the input the model has seen
  followed by something, falling back to the most frequent word it knows.
*/
static uint32_t
ChatSeed
  (const Vocab *vocab,
  const NGram *model,
  const TokenList *tokens)
{
  for ( size_t i = tokens->count; i > 0; --i )
  {
    uint32_t id = VocabLookup(vocab, tokens->items[i - 1]);
    if ( VOCAB_NO_ID != id && NGRAM_NO_ID != NGramBest(model, id) )
      return id;
  }

  uint32_t frequent = VOCAB_NO_ID;
  if ( 0 == VocabTopWords(vocab, &frequent, 1) )
    return VOCAB_NO_ID;

  return NGRAM_NO_ID == NGramBest(model, frequent) ? VOCAB_NO_ID : frequent;
}


int
ChatReply
  (const Vocab *vocab,
  const NGram *model,
  ChatSettings settings,
  const char *input,
  char *out,
  size_t size)
{
  if ( NULL == vocab || NULL == model || NULL == input || NULL == out
       || 0 == size )
    return DANA_EINVAL;

  TokenList tokens;
  int status = Tokenize(input, &tokens);
  if ( DANA_OK != status )
    return status;

  uint32_t current = ChatSeed(vocab, model, &tokens);
  TokenListFree(&tokens);

  if ( VOCAB_NO_ID == current )
    return DANA_EINVAL;

  size_t written = 0;
  out[0] = '\0';

  for ( size_t i = 0; i < settings.max_reply; ++i )
  {
    const char *word = VocabWord(vocab, current);
    if ( NULL == word )
      break;

    size_t length = strlen(word);
    size_t needed = length + (0 == written ? 1 : 2); // Space and NUL
    if ( written + needed > size )
      break;

    if ( 0 != written )
      out[written++] = ' ';
    memcpy(out + written, word, length + 1);
    written += length;

    uint32_t next = NGramSample(model, current, settings.temperature);
    if ( NGRAM_NO_ID == next )
      break;

    current = next;
  }

  return 0 == written ? DANA_EINVAL : DANA_OK;
}


int
ChatRun
  (const Vocab *vocab,
  const NGram *model,
  ChatSettings settings)
{
  if ( NULL == vocab || NULL == model )
    return DANA_EINVAL;

  char input[CHAT_INPUT_SIZE];
  char reply[CHAT_INPUT_SIZE * 4];

  LogInfo("%sDana%s knows %zu words. Ctrl-D to leave.\n\n",
          C_CYAN, C_RESET, VocabSize(vocab));

  for ( ;; )
  {
    LogInfo("%syou%s > ", C_GREEN, C_RESET);
    fflush(stdout);

    int status = CorpusReadLine(input, sizeof(input));
    if ( DANA_EEOF == status )
    {
      LogInfo("\n");
      return DANA_OK;
    }
    if ( DANA_OK != status )
    {
      LogStatus("cannot read input", "ChatRun", status);
      return status;
    }

    status = ChatReply(vocab, model, settings, input, reply, sizeof(reply));
    if ( DANA_OK != status )
    {
      LogInfo("%sdana%s > (nothing learned about those words yet)\n",
              C_CYAN, C_RESET);
      continue;
    }

    LogInfo("%sdana%s > %s\n", C_CYAN, C_RESET, reply);
  }
}
