#ifndef DANA_VOCAB_H
#define DANA_VOCAB_H

#include <stddef.h>
#include <stdint.h>

/*
  The vocabulary: every distinct word the model has seen, each with an integer
  id and an occurrence count. Past this point the rest of the program works on
  ids rather than strings.
*/

#define VOCAB_NO_ID UINT32_MAX

typedef struct Vocab Vocab;

Vocab *
VocabCreate
  (void);

void
VocabDestroy
  (Vocab *vocab);

// Number of distinct words
size_t
VocabSize
  (const Vocab *vocab);

// Number of words seen, counting repetitions
uint64_t
VocabTotalTokens
  (const Vocab *vocab);

/*
  Returns the id of `word`, adding it to the vocabulary if new, and counts one
  more occurrence. Returns VOCAB_NO_ID on allocation failure.
*/
uint32_t
VocabAdd
  (Vocab *vocab,
  const char *word);

// Returns VOCAB_NO_ID for an unknown word, without adding it
uint32_t
VocabLookup
  (const Vocab *vocab,
  const char *word);

// NULL for an out-of-range id; owned by the vocabulary
const char *
VocabWord
  (const Vocab *vocab,
  uint32_t id);

uint64_t
VocabWordCount
  (const Vocab *vocab,
  uint32_t id);

// Fills `out_ids` with the ids of the `n` most frequent words, returns how many
size_t
VocabTopWords
  (const Vocab *vocab,
  uint32_t *out_ids,
  size_t n);

#endif
