#include "Chat.h"
#include "Core.h"
#include "Log.h"
#include "NGram.h"
#include "Trainer.h"
#include "Vocab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOP_WORDS 10


static void
Usage
  (const char *program)
{
  fprintf(stderr,
          "usage:\n"
          "  %s vocab FILE   learn FILE and report what it found\n"
          "  %s chat FILE    learn FILE, then talk\n",
          program, program);
}


static int
ReportVocab
  (const Vocab *vocab,
  const NGram *model)
{
  LogInfo("%zu distinct words, %llu words read, %zu contexts\n",
          VocabSize(vocab),
          (unsigned long long)VocabTotalTokens(vocab),
          NGramContextCount(model));

  uint32_t top[TOP_WORDS];
  size_t found = VocabTopWords(vocab, top, TOP_WORDS);

  LogInfo("\nmost frequent words\n");
  for ( size_t i = 0; i < found; ++i )
    LogInfo("  %-20s %llu\n",
            VocabWord(vocab, top[i]),
            (unsigned long long)VocabWordCount(vocab, top[i]));

  if ( 0 == found )
    return DANA_OK;

  const char *word = VocabWord(vocab, top[0]);
  uint32_t next = NGramBest(model, top[0]);

  if ( NGRAM_NO_ID != next )
    LogInfo("\nafter \"%s\" the likeliest word is \"%s\" (%llu times, p=%.3f)\n",
            word,
            VocabWord(vocab, next),
            (unsigned long long)NGramPairCount(model, top[0], next),
            NGramProbability(model, top[0], next, VocabSize(vocab), 1.0));

  return DANA_OK;
}


int
main
  (int argc,
  char **argv)
{
  if ( 3 != argc )
  {
    Usage(argv[0]);
    return EXIT_FAILURE;
  }

  const char *command = argv[1];
  const char *path = argv[2];

  if ( 0 != strcmp(command, "vocab") && 0 != strcmp(command, "chat") )
  {
    Usage(argv[0]);
    return EXIT_FAILURE;
  }

  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();
  if ( NULL == vocab || NULL == model )
  {
    LogError("cannot create the model", "main");
    VocabDestroy(vocab);
    NGramDestroy(model);
    return EXIT_FAILURE;
  }

  int status = TrainOnFile(vocab, model, path);
  if ( DANA_OK != status )
  {
    LogStatus("cannot learn from the corpus", path, status);
    VocabDestroy(vocab);
    NGramDestroy(model);
    return EXIT_FAILURE;
  }

  if ( 0 == strcmp(command, "vocab") )
  {
    status = ReportVocab(vocab, model);
  }
  else
  {
    srand((unsigned int)time(NULL));
    status = ChatRun(vocab, model, ChatDefaults());
  }

  VocabDestroy(vocab);
  NGramDestroy(model);

  return DANA_OK == status ? EXIT_SUCCESS : EXIT_FAILURE;
}
