#ifndef UTILS_HASH_INTERNAL_H
#define UTILS_HASH_INTERNAL_H

#include "Utils/Hash/Hash.h"

#define LF_THRESHOLD 980
#define LF_RESIZE_TRIGGER_VALUE 700

size_t
hashfn
  (const void *key,
  const size_t size);

size_t 
Hfind_slot
  (Hash *self,
  const void *key,
  const size_t size);

bool 
Hcompare_key_entry
  (const void *key1,
  const size_t size,
  const RHEntry entry);

int 
Hrehash
  (Hash *self,
  RHEntry *TABLE);

int
Hdisplay_char
  (Hash *self);

int
Hdisplay_int
  (Hash *self);

int
Hdisplay_int64
  (Hash *self);

#endif
