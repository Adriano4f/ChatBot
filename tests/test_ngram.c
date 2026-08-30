#include "NGram.h"
#include "Trainer.h"
#include "Vocab.h"

#include "check.h"

#include <math.h>
#include <string.h>

#define CLOSE(a, b) (fabs((a) - (b)) < 1e-9)


static void
TestCounts
  (void)
{
  NGram *model = NGramCreate();
  CHECK(NULL != model);

  CHECK(0 == NGramObserve(model, 1, 2));
  CHECK(0 == NGramObserve(model, 1, 2));
  CHECK(0 == NGramObserve(model, 1, 3));

  CHECK(2 == NGramPairCount(model, 1, 2));
  CHECK(1 == NGramPairCount(model, 1, 3));
  CHECK(0 == NGramPairCount(model, 1, 4));
  CHECK(0 == NGramPairCount(model, 9, 2));
  CHECK(1 == NGramContextCount(model));
  CHECK(2 == NGramBest(model, 1));
  CHECK(NGRAM_NO_ID == NGramBest(model, 9));

  NGramDestroy(model);
}


// What it learns from text is only which word followed which
static void
TestLearnsFromText
  (void)
{
  Vocab *vocab = VocabCreate();
  NGram *model = NGramCreate();
  CHECK(NULL != vocab && NULL != model);

  CHECK(0 == TrainOnText(vocab, model,
                         "the cat sat. the cat ran. the dog sat."));

  uint32_t the = VocabLookup(vocab, "the");
  uint32_t cat = VocabLookup(vocab, "cat");
  uint32_t dog = VocabLookup(vocab, "dog");
  uint32_t sat = VocabLookup(vocab, "sat");

  CHECK(2 == NGramPairCount(model, the, cat));
  CHECK(1 == NGramPairCount(model, the, dog));
  CHECK(cat == NGramBest(model, the));
  CHECK(1 == NGramPairCount(model, cat, sat));

  // "sat the" spans the sentence break: this model has no sentence boundaries
  CHECK(1 == NGramPairCount(model, sat, the));

  NGramDestroy(model);
  VocabDestroy(vocab);
}


static void
TestProbability
  (void)
{
  NGram *model = NGramCreate();
  CHECK(NULL != model);

  NGramObserve(model, 1, 2);
  NGramObserve(model, 1, 2);
  NGramObserve(model, 1, 3);

  // Unsmoothed: 2 of the 3 words seen after 1 were the word 2
  CHECK(CLOSE(2.0 / 3.0, NGramProbability(model, 1, 2, 10, 0.0)));

  // Add-one over a vocabulary of 10: (2 + 1) / (3 + 10)
  CHECK(CLOSE(3.0 / 13.0, NGramProbability(model, 1, 2, 10, 1.0)));

  // Never seen, yet not impossible
  CHECK(CLOSE(1.0 / 13.0, NGramProbability(model, 1, 7, 10, 1.0)));
  CHECK(0.0 == NGramProbability(model, 1, 7, 10, 0.0));

  // Unknown context falls back to the smoothing mass alone
  CHECK(CLOSE(1.0 / 10.0, NGramProbability(model, 9, 2, 10, 1.0)));
  CHECK(0.0 == NGramProbability(model, 1, 2, 0, 1.0));

  NGramDestroy(model);
}


static void
TestSampling
  (void)
{
  NGram *model = NGramCreate();
  CHECK(NULL != model);

  NGramObserve(model, 1, 2);
  NGramObserve(model, 1, 2);
  NGramObserve(model, 1, 2);
  NGramObserve(model, 1, 3);

  // The successors of 1 hold 3/4 of the weight for 2 and 1/4 for 3
  CHECK(2 == NGramSampleWith(model, 1, 1.0, 0.0));
  CHECK(2 == NGramSampleWith(model, 1, 1.0, 0.7));
  CHECK(3 == NGramSampleWith(model, 1, 1.0, 0.9));
  CHECK(3 == NGramSampleWith(model, 1, 1.0, 0.999999));

  // Temperature 0 is "always the likeliest"
  CHECK(2 == NGramSampleWith(model, 1, 0.0, 0.99));

  CHECK(NGRAM_NO_ID == NGramSampleWith(model, 42, 1.0, 0.5));
  CHECK(NGRAM_NO_ID == NGramSample(model, 42, 1.0));

  uint32_t drawn = NGramSample(model, 1, 1.0);
  CHECK(2 == drawn || 3 == drawn);

  NGramDestroy(model);
}


static void
TestInvalidArguments
  (void)
{
  NGram *model = NGramCreate();

  CHECK(0 != NGramObserve(NULL, 1, 2));
  CHECK(0 != NGramObserve(model, NGRAM_NO_ID, 2));
  CHECK(0 != NGramObserve(model, 1, NGRAM_NO_ID));
  CHECK(0 != NGramTrain(model, NULL, 3));
  CHECK(0 == NGramContextCount(NULL));

  // A single token has no pair in it
  uint32_t one[] = { 5 };
  CHECK(0 == NGramTrain(model, one, 1));
  CHECK(0 == NGramContextCount(model));

  NGramDestroy(model);
  NGramDestroy(NULL);
}


int
main
  (void)
{
  TestCounts();
  TestLearnsFromText();
  TestProbability();
  TestSampling();
  TestInvalidArguments();

  return 0;
}
