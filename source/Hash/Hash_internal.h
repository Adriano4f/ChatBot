#ifndef _HASH_INTERNAL_
#define _HASH_INTERNAL_

#include "Hash.h"

#define LF_RESIZE_TRIGGER_VALUE 700 // Per mille, the table grows above 0.7

size_t
hashfn
  (const void *key,
  const size_t size);

size_t 
Hfind_slot
  (Hash *this,
  const void *key,
  const size_t size);

bool 
Hcompare_key_entry
  (const void *key1,
  const size_t size,
  const RHEntry entry);

int 
Hrehash
  (Hash *this,
  RHEntry *TABLE,
  const size_t OLD_CAPACITY);

int
Hdisplay_char
  (Hash *this);
int
Hdisplay_int
  (Hash *this);
int
Hdisplay_int64
  (Hash *this);

#endif

