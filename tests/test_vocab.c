#include "Vocab.h"

#include "check.h"

#include <stdio.h>
#include <string.h>


static void
TestIdsAreStable
  (void)
{
  Vocab *vocab = VocabCreate();
  CHECK(NULL != vocab);

  uint32_t the = VocabAdd(vocab, "the");
  uint32_t cat = VocabAdd(vocab, "cat");

  CHECK(the != cat);
  CHECK(the == VocabAdd(vocab, "the"));
  CHECK(the == VocabLookup(vocab, "the"));
  CHECK(2 == VocabSize(vocab));
  CHECK(3 == VocabTotalTokens(vocab));

  CHECK(0 == strcmp("the", VocabWord(vocab, the)));
  CHECK(2 == VocabWordCount(vocab, the));
  CHECK(1 == VocabWordCount(vocab, cat));

  CHECK(VOCAB_NO_ID == VocabLookup(vocab, "dog"));
  CHECK(2 == VocabSize(vocab)); // Lookup must not add

  VocabDestroy(vocab);
}


static void
TestTopWords
  (void)
{
  Vocab *vocab = VocabCreate();
  CHECK(NULL != vocab);

  for ( int i = 0; i < 5; ++i )
    VocabAdd(vocab, "common");
  for ( int i = 0; i < 3; ++i )
    VocabAdd(vocab, "middle");
  VocabAdd(vocab, "rare");

  uint32_t top[2];
  CHECK(2 == VocabTopWords(vocab, top, 2));
  CHECK(0 == strcmp("common", VocabWord(vocab, top[0])));
  CHECK(0 == strcmp("middle", VocabWord(vocab, top[1])));

  // Asking for more than there is returns what there is
  uint32_t all[10];
  CHECK(3 == VocabTopWords(vocab, all, 10));

  VocabDestroy(vocab);
}


// The vocabulary array has to grow past its initial capacity
static void
TestManyWords
  (void)
{
  Vocab *vocab = VocabCreate();
  CHECK(NULL != vocab);

  for ( int i = 0; i < 5000; ++i )
  {
    char word[32];
    snprintf(word, sizeof(word), "w%d", i);
    CHECK((uint32_t)i == VocabAdd(vocab, word));
  }

  CHECK(5000 == VocabSize(vocab));
  for ( int i = 0; i < 5000; ++i )
  {
    char word[32];
    snprintf(word, sizeof(word), "w%d", i);
    CHECK((uint32_t)i == VocabLookup(vocab, word));
  }

  VocabDestroy(vocab);
}


static void
TestInvalidArguments
  (void)
{
  Vocab *vocab = VocabCreate();

  CHECK(VOCAB_NO_ID == VocabAdd(vocab, NULL));
  CHECK(VOCAB_NO_ID == VocabAdd(vocab, ""));
  CHECK(VOCAB_NO_ID == VocabAdd(NULL, "word"));
  CHECK(NULL == VocabWord(vocab, 12345));
  CHECK(0 == VocabWordCount(vocab, 12345));
  CHECK(0 == VocabSize(NULL));
  CHECK(0 == VocabTopWords(vocab, NULL, 3));

  VocabDestroy(vocab);
  VocabDestroy(NULL);
}


int
main
  (void)
{
  TestIdsAreStable();
  TestTopWords();
  TestManyWords();
  TestInvalidArguments();

  return 0;
}
