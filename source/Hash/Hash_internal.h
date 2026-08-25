#include "Hash.h"

#define LF_THRESHOLD 980
#define LF_RESIZE_TRIGGER_VALUE 700
#define HASH_INVALID_SLOT SIZE_MAX
#define HASH_LOAD_FACTOR(size, capacity) ((size)*1000/(capacity))

size_t
hashfn
  (const void *key,
  const size_t size);

size_t
Hbucket
  (const Hash *this,
  const void *key,
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
  RHEntry *TABLE);

int
Hdisplay_entries
  (Hash *this,
  size_t elemsize,
  bool use_entry_sizes);
