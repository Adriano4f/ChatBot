#include "NGram_internal.h"

#include "Core.h"

#include <stdlib.h>


NGram *
NGramCreate
  (void)
{
  NGram *model = (NGram *)DanaAlloc(sizeof(NGram), "NGramCreate");
  if ( NULL == model )
    return NULL;

  model->contexts = HashMapCreate(sizeof(Context));
  if ( NULL == model->contexts )
  {
    free(model);
    return NULL;
  }

  return model;
}


void
NGramDestroy
  (NGram *model)
{
  if ( NULL == model )
    return;

  size_t cursor = 0;
  HashMapEntry entry;
  while ( HashMapNext(model->contexts, &cursor, &entry) )
  {
    Context *context = (Context *)entry.value;
    free(context->successors);
    HashMapDestroy(context->index);
  }

  HashMapDestroy(model->contexts);
  free(model);
}


size_t
NGramContextCount
  (const NGram *model)
{
  return NULL == model ? 0 : HashMapCount(model->contexts);
}


const Context *
NGramFindContext
  (const NGram *model,
  uint32_t previous)
{
  if ( NULL == model )
    return NULL;

  return (const Context *)HashMapGet(model->contexts, &previous,
                                     sizeof(previous));
}


static Context *
NGramContextFor
  (NGram *model,
  uint32_t previous)
{
  Context *context = (Context *)HashMapGet(model->contexts, &previous,
                                           sizeof(previous));
  if ( NULL != context )
    return context;

  Context fresh;
  fresh.capacity = NGRAM_SUCCESSORS_INITIAL_CAPACITY;
  fresh.count = 0;
  fresh.total = 0;
  fresh.successors = (Successor *)DanaAlloc(
      fresh.capacity * sizeof(Successor), "NGramContextFor");
  fresh.index = HashMapCreate(sizeof(size_t));

  if ( NULL == fresh.successors || NULL == fresh.index )
  {
    free(fresh.successors);
    HashMapDestroy(fresh.index);
    return NULL;
  }

  context = (Context *)HashMapPut(model->contexts, &previous, sizeof(previous),
                                  &fresh);
  if ( NULL == context )
  {
    free(fresh.successors);
    HashMapDestroy(fresh.index);
    return NULL;
  }

  return context;
}


int
NGramObserve
  (NGram *model,
  uint32_t previous,
  uint32_t next)
{
  if ( NULL == model || NGRAM_NO_ID == previous || NGRAM_NO_ID == next )
    return DANA_EINVAL;

  Context *context = NGramContextFor(model, previous);
  if ( NULL == context )
    return DANA_ENOMEM;

  size_t *slot = (size_t *)HashMapGet(context->index, &next, sizeof(next));
  if ( NULL != slot )
  {
    ++context->successors[*slot].count;
    ++context->total;
    return DANA_OK;
  }

  if ( context->count == context->capacity )
  {
    size_t capacity = context->capacity * 2;
    Successor *successors = (Successor *)DanaRealloc(
        context->successors, capacity * sizeof(Successor), "NGramObserve");
    if ( NULL == successors )
      return DANA_ENOMEM;

    context->successors = successors;
    context->capacity = capacity;
  }

  size_t index = context->count;
  if ( NULL == HashMapPut(context->index, &next, sizeof(next), &index) )
    return DANA_ENOMEM;

  context->successors[index].id = next;
  context->successors[index].count = 1;
  ++context->count;
  ++context->total;

  return DANA_OK;
}


int
NGramTrain
  (NGram *model,
  const uint32_t *ids,
  size_t count)
{
  if ( NULL == model || NULL == ids )
    return DANA_EINVAL;

  for ( size_t i = 0; i + 1 < count; ++i )
  {
    int status = NGramObserve(model, ids[i], ids[i + 1]);
    if ( DANA_OK != status )
      return status;
  }

  return DANA_OK;
}


uint64_t
NGramPairCount
  (const NGram *model,
  uint32_t previous,
  uint32_t next)
{
  const Context *context = NGramFindContext(model, previous);
  if ( NULL == context )
    return 0;

  size_t *slot = (size_t *)HashMapGet(context->index, &next, sizeof(next));

  return NULL == slot ? 0 : context->successors[*slot].count;
}
