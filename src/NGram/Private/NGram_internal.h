#ifndef DANA_NGRAM_INTERNAL_H
#define DANA_NGRAM_INTERNAL_H

#include "HashMap.h"
#include "NGram.h"

#define NGRAM_SUCCESSORS_INITIAL_CAPACITY 4

typedef struct Successor
{
  uint32_t id;
  uint64_t count;
} Successor;

/*
  Everything that followed one context word: the counts as an array so sampling
  can walk them, plus an id -> index map so counting a pair stays O(1).
*/
typedef struct Context
{
  Successor *successors;
  size_t     count;
  size_t     capacity;
  uint64_t   total;
  HashMap   *index; // uint32_t successor id -> size_t index into successors
} Context;

struct NGram
{
  HashMap *contexts; // uint32_t word id -> Context
};

const Context *
NGramFindContext
  (const NGram *model,
  uint32_t previous);

#endif
