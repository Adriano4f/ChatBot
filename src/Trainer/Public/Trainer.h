#ifndef DANA_TRAINER_H
#define DANA_TRAINER_H

#include <stddef.h>

#include "NGram.h"
#include "Vocab.h"

/*
  Turns text into a trained model: tokenize, give every word an id, then count
  which word follows which. This is the only place where learning happens.
*/

int
TrainOnText
  (Vocab *vocab,
  NGram *model,
  const char *text);

int
TrainOnFile
  (Vocab *vocab,
  NGram *model,
  const char *path);

#endif
