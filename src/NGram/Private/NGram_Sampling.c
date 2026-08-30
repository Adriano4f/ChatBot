#include "NGram_internal.h"

#include <math.h>
#include <stdlib.h>


double
NGramProbability
  (const NGram *model,
  uint32_t previous,
  uint32_t next,
  size_t vocab_size,
  double k)
{
  if ( 0 == vocab_size )
    return 0.0;

  /*
    Add-k smoothing: every word is treated as if it had been seen k times more
    than it was, so a pair never seen still gets a probability above zero
    instead of making the whole sentence impossible.
  */
  uint64_t pair = NGramPairCount(model, previous, next);
  const Context *context = NGramFindContext(model, previous);
  double total = NULL == context ? 0.0 : (double)context->total;
  double mass = total + k * (double)vocab_size;

  if ( !(mass > 0.0) )
    return 0.0;

  return ((double)pair + k) / mass;
}


uint32_t
NGramBest
  (const NGram *model,
  uint32_t previous)
{
  const Context *context = NGramFindContext(model, previous);
  if ( NULL == context || 0 == context->count )
    return NGRAM_NO_ID;

  size_t best = 0;
  for ( size_t i = 1; i < context->count; ++i )
    if ( context->successors[i].count > context->successors[best].count )
      best = i;

  return context->successors[best].id;
}


uint32_t
NGramSampleWith
  (const NGram *model,
  uint32_t previous,
  double temperature,
  double uniform)
{
  const Context *context = NGramFindContext(model, previous);
  if ( NULL == context || 0 == context->count || 0 == context->total )
    return NGRAM_NO_ID;

  if ( temperature <= 0.0 )
    return NGramBest(model, previous);

  /*
    Weights are the counts raised to 1/temperature: above 1.0 the differences
    flatten out and the text gets more surprising, below 1.0 they sharpen and
    it repeats the most common continuations.
  */
  double weights = 0.0;
  for ( size_t i = 0; i < context->count; ++i )
    weights += pow((double)context->successors[i].count, 1.0 / temperature);

  if ( !(weights > 0.0) )
    return NGramBest(model, previous);

  double target = uniform * weights;
  double walked = 0.0;
  for ( size_t i = 0; i < context->count; ++i )
  {
    walked += pow((double)context->successors[i].count, 1.0 / temperature);
    if ( walked > target )
      return context->successors[i].id;
  }

  return context->successors[context->count - 1].id;
}


uint32_t
NGramSample
  (const NGram *model,
  uint32_t previous,
  double temperature)
{
  double uniform = (double)rand() / ((double)RAND_MAX + 1.0);

  return NGramSampleWith(model, previous, temperature, uniform);
}
