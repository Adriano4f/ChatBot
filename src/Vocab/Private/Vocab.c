#include "Vocab.h"

#include "Core.h"
#include "HashMap.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define VOCAB_INITIAL_CAPACITY 256

typedef struct VocabWordEntry
{
  char    *word;
  uint64_t count;
} VocabWordEntry;

struct Vocab
{
  HashMap        *ids; // word -> uint32_t id
  VocabWordEntry *words;
  size_t          count;
  size_t          capacity;
  uint64_t        total;
};


Vocab *
VocabCreate
  (void)
{
  Vocab *vocab = (Vocab *)DanaAlloc(sizeof(Vocab), "VocabCreate");
  if ( NULL == vocab )
    return NULL;

  vocab->ids = HashMapCreate(sizeof(uint32_t));
  vocab->words = (VocabWordEntry *)DanaAlloc(
      VOCAB_INITIAL_CAPACITY * sizeof(VocabWordEntry), "VocabCreate");

  if ( NULL == vocab->ids || NULL == vocab->words )
  {
    HashMapDestroy(vocab->ids);
    free(vocab->words);
    free(vocab);
    return NULL;
  }

  vocab->count = 0;
  vocab->capacity = VOCAB_INITIAL_CAPACITY;
  vocab->total = 0;

  return vocab;
}


void
VocabDestroy
  (Vocab *vocab)
{
  if ( NULL == vocab )
    return;

  for ( size_t i = 0; i < vocab->count; ++i )
    free(vocab->words[i].word);

  free(vocab->words);
  HashMapDestroy(vocab->ids);
  free(vocab);
}


size_t
VocabSize
  (const Vocab *vocab)
{
  return NULL == vocab ? 0 : vocab->count;
}


uint64_t
VocabTotalTokens
  (const Vocab *vocab)
{
  return NULL == vocab ? 0 : vocab->total;
}


static int
VocabReserve
  (Vocab *vocab)
{
  if ( vocab->count < vocab->capacity )
    return DANA_OK;

  size_t capacity = vocab->capacity * 2;
  VocabWordEntry *words = (VocabWordEntry *)DanaRealloc(
      vocab->words, capacity * sizeof(VocabWordEntry), "VocabReserve");
  if ( NULL == words )
    return DANA_ENOMEM;

  vocab->words = words;
  vocab->capacity = capacity;

  return DANA_OK;
}


uint32_t
VocabAdd
  (Vocab *vocab,
  const char *word)
{
  if ( NULL == vocab || NULL == word || '\0' == *word )
    return VOCAB_NO_ID;

  size_t size = strlen(word) + 1;
  uint32_t *existing = (uint32_t *)HashMapGet(vocab->ids, word, size);
  if ( NULL != existing )
  {
    ++vocab->words[*existing].count;
    ++vocab->total;
    return *existing;
  }

  if ( DANA_OK != VocabReserve(vocab) )
    return VOCAB_NO_ID;

  char *copy = (char *)DanaAlloc(size, "VocabAdd");
  if ( NULL == copy )
    return VOCAB_NO_ID;
  memcpy(copy, word, size);

  uint32_t id = (uint32_t)vocab->count;
  if ( NULL == HashMapPut(vocab->ids, word, size, &id) )
  {
    free(copy);
    return VOCAB_NO_ID;
  }

  vocab->words[id].word = copy;
  vocab->words[id].count = 1;
  ++vocab->count;
  ++vocab->total;

  return id;
}


uint32_t
VocabLookup
  (const Vocab *vocab,
  const char *word)
{
  if ( NULL == vocab || NULL == word )
    return VOCAB_NO_ID;

  uint32_t *id = (uint32_t *)HashMapGet(vocab->ids, word, strlen(word) + 1);

  return NULL == id ? VOCAB_NO_ID : *id;
}


const char *
VocabWord
  (const Vocab *vocab,
  uint32_t id)
{
  if ( NULL == vocab || id >= vocab->count )
    return NULL;

  return vocab->words[id].word;
}


uint64_t
VocabWordCount
  (const Vocab *vocab,
  uint32_t id)
{
  if ( NULL == vocab || id >= vocab->count )
    return 0;

  return vocab->words[id].count;
}


size_t
VocabTopWords
  (const Vocab *vocab,
  uint32_t *out_ids,
  size_t n)
{
  if ( NULL == vocab || NULL == out_ids || 0 == n )
    return 0;

  // Selection of the n best, n being a handful: no need to sort the vocabulary
  size_t found = 0;
  for ( size_t rank = 0; rank < n; ++rank )
  {
    uint32_t best = VOCAB_NO_ID;

    for ( uint32_t id = 0; id < (uint32_t)vocab->count; ++id )
    {
      bool taken = false;
      for ( size_t i = 0; i < found; ++i )
        taken = taken || out_ids[i] == id;
      if ( taken )
        continue;

      if ( VOCAB_NO_ID == best
           || vocab->words[id].count > vocab->words[best].count )
        best = id;
    }

    if ( VOCAB_NO_ID == best )
      break;

    out_ids[found] = best;
    ++found;
  }

  return found;
}
