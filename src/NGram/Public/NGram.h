#ifndef DANA_NGRAM_H
#define DANA_NGRAM_H

#include <stddef.h>
#include <stdint.h>

#include "Vocab.h"

/*
  A bigram language model: for every word, how often each other word followed
  it. That is the whole of what it learns, and it learns it by counting, not
  from rules. Generation is then "given the previous word, draw the next one
  from the distribution of what followed it in the training text".
*/

#define NGRAM_NO_ID VOCAB_NO_ID

typedef struct NGram NGram;

NGram *
NGramCreate
  (void);

void
NGramDestroy
  (NGram *model);

// Number of distinct words seen as a context
size_t
NGramContextCount
  (const NGram *model);

int
NGramObserve
  (NGram *model,
  uint32_t previous,
  uint32_t next);

// Counts every adjacent pair in `ids`
int
NGramTrain
  (NGram *model,
  const uint32_t *ids,
  size_t count);

uint64_t
NGramPairCount
  (const NGram *model,
  uint32_t previous,
  uint32_t next);

// P(next | previous), with add-k smoothing over `vocab_size` words
double
NGramProbability
  (const NGram *model,
  uint32_t previous,
  uint32_t next,
  size_t vocab_size,
  double k);

// The most frequent successor of `previous`, NGRAM_NO_ID if it has none
uint32_t
NGramBest
  (const NGram *model,
  uint32_t previous);

/*
  Draws a successor of `previous`. `temperature` flattens the distribution
  above 1.0 and sharpens it below; `uniform` is a random number in [0, 1).
  Returns NGRAM_NO_ID when `previous` was never seen followed by anything.
*/
uint32_t
NGramSampleWith
  (const NGram *model,
  uint32_t previous,
  double temperature,
  double uniform);

// As NGramSampleWith, drawing `uniform` from rand()
uint32_t
NGramSample
  (const NGram *model,
  uint32_t previous,
  double temperature);

#endif
