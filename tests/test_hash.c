#include "Hash.h"

#include "check.h"
#include <stdio.h>
#include <string.h>

#define KEYS 2000

static char keys[KEYS][16];
static int  values[KEYS];

static size_t
key_size
  (const char *key)
{
  return strlen(key);
}


static void
test_insert_and_fetch
  (void)
{
  Hash *h = Hinit();
  CHECK(NULL != h);

  for ( int i = 0; i < KEYS; ++i )
  {
    snprintf(keys[i], sizeof(keys[i]), "k%d", i);
    values[i] = i;
    CHECK(H_SUCESS == INSERT(h, keys[i], key_size(keys[i]), &values[i], sizeof(int)));
  }

  CHECK(KEYS == (int)h->SIZE);
  CHECK(h->CAPACITY > h->SIZE); // Grew while inserting

  for ( int i = 0; i < KEYS; ++i )
  {
    const RHEntry *entry = FETCH(h, keys[i], key_size(keys[i]));
    CHECK(NULL != entry);
    CHECK(i == *(const int *)entry->value);
  }

  DESTROY(h);
}


static void
test_overwrite_keeps_one_entry
  (void)
{
  Hash *h = Hinit();
  CHECK(NULL != h);

  char key[] = "droga";
  int first = 1;
  int second = 2;

  CHECK(H_SUCESS == INSERT(h, key, sizeof(key), &first, sizeof(int)));
  CHECK(H_SUCESS == INSERT(h, key, sizeof(key), &second, sizeof(int)));
  CHECK(1 == h->SIZE);
  CHECK(second == *(const int *)FETCH(h, key, sizeof(key))->value);

  DESTROY(h);
}


static void
test_delete_keeps_the_other_keys_reachable
  (void)
{
  /*
    Regression test: probing used to stop at the first tombstone, which made
    every key stored behind a deleted one unreachable.
  */
  Hash *h = Hinit();
  CHECK(NULL != h);

  for ( int i = 0; i < KEYS; ++i )
  {
    snprintf(keys[i], sizeof(keys[i]), "k%d", i);
    values[i] = i;
    CHECK(H_SUCESS == INSERT(h, keys[i], key_size(keys[i]), &values[i], sizeof(int)));
  }

  for ( int i = 0; i < KEYS; i += 2 )
    DELETE(h, keys[i], key_size(keys[i]));

  for ( int i = 0; i < KEYS; ++i )
  {
    const RHEntry *entry = FETCH(h, keys[i], key_size(keys[i]));
    if ( 0 == i % 2 )
      CHECK(NULL == entry); // Deleted
    else
    {
      CHECK(NULL != entry);
      CHECK(i == *(const int *)entry->value);
    }
  }

  DESTROY(h);
}


static void
test_missing_key_and_invalid_arguments
  (void)
{
  Hash *h = Hinit();
  CHECK(NULL != h);

  char key[] = "absent";
  CHECK(NULL == FETCH(h, key, sizeof(key)));
  CHECK(NULL == Hfetch(NULL, key, sizeof(key)));
  CHECK(H_INVALID_ARG == Hinsert(NULL, key, sizeof(key), key, sizeof(key)));
  CHECK(H_INVALID_ARG == Hinsert(h, NULL, 0, key, sizeof(key)));
  CHECK(H_INVALID_ARG == Hresize(NULL));
  CHECK(H_INVALID_ARG == Hdisplay(NULL, CHAR));

  Hdelete(NULL, key, sizeof(key)); // Must not crash
  Hdestroy(NULL);

  DESTROY(h);
}


int
main
  (void)
{
  test_insert_and_fetch();
  test_overwrite_keeps_one_entry();
  test_delete_keeps_the_other_keys_reachable();
  test_missing_key_and_invalid_arguments();

  puts("test_hash: all assertions passed");
  return 0;
}
