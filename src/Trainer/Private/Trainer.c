#include "Trainer.h"

#include "Core.h"
#include "Corpus.h"
#include "Tokenizer.h"

#include <stdlib.h>


int
TrainOnText
  (Vocab *vocab,
  NGram *model,
  const char *text)
{
  if ( NULL == vocab || NULL == model || NULL == text )
    return DANA_EINVAL;

  TokenList tokens;
  int status = Tokenize(text, &tokens);
  if ( DANA_OK != status )
    return status;

  uint32_t previous = VOCAB_NO_ID;
  for ( size_t i = 0; i < tokens.count; ++i )
  {
    uint32_t id = VocabAdd(vocab, tokens.items[i]);
    if ( VOCAB_NO_ID == id )
    {
      status = DANA_ENOMEM;
      break;
    }

    if ( VOCAB_NO_ID != previous )
    {
      status = NGramObserve(model, previous, id);
      if ( DANA_OK != status )
        break;
    }

    previous = id;
  }

  TokenListFree(&tokens);

  return status;
}


int
TrainOnFile
  (Vocab *vocab,
  NGram *model,
  const char *path)
{
  char *text = NULL;
  size_t size = 0;

  int status = CorpusRead(path, &text, &size);
  if ( DANA_OK != status )
    return status;

  status = TrainOnText(vocab, model, text);
  free(text);

  return status;
}
